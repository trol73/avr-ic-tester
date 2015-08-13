#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
from analyser import Analyser
from generator import DataGenerator, convert_pin
import parser

from classes import Chip


OPTIMIZE_CMD_ALL = True             # использовать CMD_SET_ALL вместо CMD_SET
OPTIMIZE_CMD_TEST = True            # использовать CMD_TEST_ALL вместо CMD_TEST везде, где это возможно
OPTIMIZE_LAST_PULSE = True          # использовать команду CMD_LAST_PULSE везде, где это возможно
OPTIMIZE_SET_AND_TEST = True        # использовать команду CMD_SET_AND_TEST вместо сочетания CMD_SET_ALL + CMD_TEST
OPTIMIZE_LAST_PULSE_AND_TEST = True # использовать команду CMD_LAST_PULSE_AND_TEST вместо сочетания CMD_LAST_PULSE + CMD_TEST

__author__ = 'trol'

src = 'data.ic'
out = '../ic-tester/data.h'

if len(sys.argv) == 2:
    src = sys.argv[1]

print 'compile', src, 'to', out


chips = []

# загружаем файл
f = open(src, 'r')
for s in f:
    s = s.strip()
    l = len(s)
    if l == 0:
        continue
    if l == 1 and (s[0] == '\n' or s[0] == '\r'):
        continue
    if s[0] == '#':
        continue

    if s[l - 1] == '\n':
        s = s[:l - 1]

    if s.startswith('CHIP['):
        chip = Chip()
        parser.load_line(chip, s)
        chips.append(chip)
    else:
        parser.load_line(chips[len(chips) - 1], s)

f.close()


def compile_chip(chip, g):
    """
        Компилирует данные для микросхемы
    :param g:
    """

    analyser = Analyser(chip.pins, chip.name)
    g.add_chip(chip.name)
    first_command_index = len(g.commands) - 1
    g.add_command('CMD_RESET_FULL')
    inputs = chip.inputs
    for power in chip.powerPlus:
        inputs.append(power)
    for power in chip.powerMinus:
        inputs.append(power)

    g.add_command_mask_1('CMD_INIT', inputs, chip.pins)
    analyser.set_ddr(inputs)

    # команды
    for cmd in chip.commands:
        if cmd.name == 'set':
            pins0 = cmd.lst0
            for power in chip.powerMinus:
                pins0.append(power)

            pins1 = cmd.lst1
            for power in chip.powerPlus:
                pins1.append(power)
            for pullUp in chip.pullUpOutputs:
                pins1.append(pullUp)
            analyser.set_pins_to_0(pins0)
            analyser.set_pins_to_1(pins1)
            if OPTIMIZE_CMD_ALL:
                g.add_command_mask_1('CMD_SET_ALL', analyser.get_levels_mask(), chip.pins)
            else:
                g.add_command_mask_2('CMD_SET', pins0, pins1, chip.pins)

        elif cmd.name == 'test':
            if OPTIMIZE_CMD_TEST:
                optimized_mask = analyser.get_test_all_mask(cmd.lst0, cmd.lst1)
            else:
                optimized_mask = None

            if optimized_mask is None:
                g.add_command_mask_2('CMD_TEST', cmd.lst0, cmd.lst1, chip.pins)
            else:
                g.add_command_mask_1('CMD_TEST_ALL', optimized_mask, chip.pins)

        elif cmd.name == 'set+test':
            pins0 = cmd.lst0
            for power in chip.powerMinus:
                pins0.append(power)

            pins1 = cmd.lst1
            for power in chip.powerPlus:
                pins1.append(power)
            for pullUp in chip.pullUpOutputs:
                pins1.append(pullUp)

            if OPTIMIZE_CMD_ALL:
                g.add_command_mask_1('CMD_SET_ALL', analyser.get_levels_mask(), chip.pins)
            else:
                g.add_command_mask_2('CMD_SET', pins0, pins1, chip.pins)
            analyser.set_pins_to_0(pins0)
            analyser.set_pins_to_1(pins1)

            if OPTIMIZE_CMD_TEST:
                optimized_mask = analyser.get_test_all_mask(cmd.lst0_2, cmd.lst1_2)
            else:
                optimized_mask = None

            if optimized_mask is None:
                g.add_command_mask_2('CMD_TEST', cmd.lst0_2, cmd.lst1_2, chip.pins)
            else:
                g.add_command_mask_1('CMD_TEST_ALL', optimized_mask, chip.pins)

        elif cmd.name == 'pulse+':
            if analyser.pulse(cmd.pin, '+'):
                g.add_command('CMD_LAST_PULSE')
            else:
                g.add_command('CMD_PULSE_PLUS', convert_pin(cmd.pin, chip.pins, 28))

        elif cmd.name == 'pulse-':
            if analyser.pulse(cmd.pin, '-'):
                g.add_command('CMD_LAST_PULSE')
            else:
                g.add_command('CMD_PULSE_MINUS', convert_pin(cmd.pin, chip.pins, 28))

        elif cmd.name == 'config':
            inputs = cmd.lst0
            for power in chip.powerPlus:
                inputs.append(power)
            for power in chip.powerMinus:
                inputs.append(power)
            chip.inputs = cmd.lst0
            chip.outputs = cmd.lst1
            g.add_command_mask_1('CMD_INIT', inputs, chip.pins)
            analyser.set_ddr(inputs)

    g.add_command('CMD_END')
    # проходимся по всем команам этой МС и выполняем оптимизации

    while True:
        optimized = False
        for i in range(first_command_index, len(g.commands)):
            cmd = g.commands[i]
            if isinstance(cmd, (list, tuple)):
                cmd_name = cmd[0]
            else:
                continue
            if i+1 < len(g.commands):
                cmd_next = g.commands[i+1]
                cmd_next_name = cmd_next[0]
            else:
                break
            #print cmd_name, cmd_next_name
            if OPTIMIZE_SET_AND_TEST and cmd_name.startswith('CMD_SET_ALL_') and cmd_next_name.startswith('CMD_TEST_ALL_'):
                optimized = True
                print g.commands[i]
                g.commands[i][0] = 'CMD_SET_ALL_AND_TEST_' + cmd_next_name[len('CMD_TEST_ALL_'):]
                for j in range(1, len(cmd_next)):
                    g.commands[i].append(cmd_next[j])
                print g.commands[i]
                del g.commands[i+1]
                break
            if OPTIMIZE_LAST_PULSE_AND_TEST and cmd_name == 'CMD_LAST_PULSE' and cmd_next_name.startswith('CMD_TEST_ALL_'):
                g.commands[i+1][0] = 'CMD_LAST_PULSE_AND_TEST_' + cmd_next_name[len('CMD_TEST_ALL_'):]
                print g.commands[i+1]
                del g.commands[i]
                optimized = True
                break
            # CMD_SET_ALL_16, CMD_TEST_ALL_16  -> CMD_SET_AND_TEST_ALL
            # CMD_LAST_PULSE, CMD_TEST_ALL_16  -> CMD_LAST_PULSE_AND_TEST_ALL
        if not optimized:
            break
        #first_command_index

g = DataGenerator()

for chip in chips:
    #chip.show()
    compile_chip(chip, g)

g.generate(out)

print '-------------[Chips]--------------------'
for chip in chips:
    print chip.name.decode('cp1251').encode('utf8')
print '----------------------------------------'


print 'Total chips: ', len(chips)
print 'Data size: ', g.size
