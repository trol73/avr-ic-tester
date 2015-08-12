#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys
from analyser import Analyser
from generator import DataGenerator, convert_pin
import parser

from classes import Chip


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

    analyser = Analyser(chip.pins)
    g.add_chip(chip.name)
    g.add_command('CMD_RESET_FULL')
    inputs = chip.inputs
    for power in chip.powerPlus:
        inputs.append(power)
    for power in chip.powerMinus:
        inputs.append(power)

    g.add_command_mask_1('CMD_INIT', inputs, chip.pins)

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
            g.add_command_mask_2('CMD_SET', pins0, pins1, chip.pins)
            analyser.set_pins_to_0(pins0)
            analyser.set_pins_to_1(pins1)

        elif cmd.name == 'test':
            g.add_command_mask_2('CMD_TEST', cmd.lst0, cmd.lst1, chip.pins)

        elif cmd.name == 'set+test':
            pins0 = cmd.lst0
            for power in chip.powerMinus:
                pins0.append(power)

            pins1 = cmd.lst1
            for power in chip.powerPlus:
                pins1.append(power)
            for pullUp in chip.pullUpOutputs:
                pins1.append(pullUp)

            g.add_command_mask_2('CMD_SET', pins0, pins1, chip.pins)
            analyser.set_pins_to_0(pins0)
            analyser.set_pins_to_1(pins1)
            g.add_command_mask_2('CMD_TEST', cmd.lst0_2, cmd.lst1_2, chip.pins)

        elif cmd.name == 'pulse+':
            g.add_command('CMD_PULSE_PLUS', convert_pin(cmd.pin, chip.pins, 28))
            analyser.set_pin(cmd.pin, 1)

        elif cmd.name == 'pulse-':
            g.add_command('CMD_PULSE_MINUS', convert_pin(cmd.pin, chip.pins, 28))
            analyser.set_pin(cmd.pin, 0)

        elif cmd.name == 'config':
            inputs = cmd.lst0
            for power in chip.powerPlus:
                inputs.append(power)
            for power in chip.powerMinus:
                inputs.append(power)
            chip.inputs = cmd.lst0
            chip.outputs = cmd.lst1
            g.add_command_mask_1('CMD_INIT', inputs, chip.pins)

    g.add_command('CMD_END')

g = DataGenerator()

for chip in chips:
    #chip.show()
    compile_chip(chip, g)

g.generate(out)

print '-------------[Chips]--------------------'
for chip in chips:
    print chip.name
print '----------------------------------------'


print 'Total chips: ', len(chips)
print 'Data size: ', g.size
