# -*- coding: utf-8 -*-
import sys

from classes import Command

__author__ = 'trol'


def load_line(chip, line):

    if line.startswith('CHIP['):
        pins = line[len('CHIP['):]
        pins = pins[:pins.find(']')]
        chip.pins = int(pins)
        name = line[line.find("'"):]
        if name[0] != "'" or name[len(name) - 1] != "'":
            error('name expected')
        chip.name = name[1:-1]

    elif line.startswith('POWER:'):
        power = line[len('POWER:'):].split(' ')
        for p in power:
            if len(p.strip()) == 0:
                continue
            if p[0] == '-':
                chip.powerMinus.append(int(p[1:]))
            elif p[0] == '+':
                chip.powerPlus.append(int(p[1:]))

    elif line.startswith('IN:'):
        inputs = line[len('IN:'):].split(',')
        for inp in inputs:
            name = inp.strip()
            if len(name) == 0:
                continue
            n = int(name)
            if n > chip.pins:
                error('wrong pin number ', n, ' for DIP-', chip.pins)
            chip.inputs.append(n)
        chip.currentInputs = chip.inputs

    elif line.startswith('OUT:'):
        outputs = line[len('OUT:'):].split(',')
        for out in outputs:
            name = out.strip()
            if len(name) == 0:
                continue
            if name[0] == '@':
                n = int(name[1:])
                if n > chip.pins:
                    error('wrong pin number ', n, ' for DIP-', chip.pins)
                chip.outputs.append(n)
                chip.pullUpOutputs.append(n)
            else:
                n = int(name)
                if n > chip.pins:
                    error('wrong pin number ', n, ' for DIP-', chip.pins)
                chip.outputs.append(n)
        chip.currentOutputs = chip.outputs

    elif line.startswith('VOID:'):
        voids = line[len('VOID:'):].split(',')
        for v in voids:
            name = v.strip()
            if len(name) == 0:
                continue
            n = int(name)
            if n > chip.pins:
                error('wrong pin number ', n, ' for DIP-', chip.pins)
                sys.exit(-1)
            chip.void.append(n)

    elif line.startswith('SET:'):
        cmd = Command('set')
        sc = line[len('SET:'):].strip()
        if sc.find('->') > 0:
            lst = sc.split(';')
            for lsts in lst:
                itm = lsts.strip()
                level = itm.split('->')[0].strip()
                pins = itm.split('->')[1].strip()
                if level == '0':
                    cmd.lst0 = str_to_int_list(pins)
                elif level == '1':
                    cmd.lst1 = str_to_int_list(pins)
                else:
                    error('invalid level ', level)
        else:
            sc = sc.replace(':', '')
            if len(sc) == len(chip.currentInputs):
                index = 0
                list1 = ''
                list0 = ''
                for inp in chip.currentInputs:
                    if sc[index] == '1':
                        list1 = list1 + str(inp) + ','
                    elif sc[index] == '0':
                        list0 = list0 + str(inp) + ','
                    else:
                        error('wrong level ', sc[index])
                    index += 1
                cmd.lst0 = str_to_int_list(list0)
                cmd.lst1 = str_to_int_list(list1)
            else:
                error('SET syntax error')

        chip.commands.append(cmd)

    elif line.startswith('TEST:'):
        cmd = Command('test')
        sc = line[len('TEST:'):].strip()
        if sc.find('->') > 0:
            lst = sc.split(';')
            for lsts in lst:
                itm = lsts.strip()
                pins = itm.split('->')[0].strip()
                level = itm.split('->')[1].strip()
                if level == '0':
                    cmd.lst0 = str_to_int_list(pins)
                elif level == '1':
                    cmd.lst1 = str_to_int_list(pins)
                else:
                    error('invalid level ', level)
        elif sc.find('=>') > 0:
            cmd.name = 'set+test'
            # команда вида TEST: xxxx => yyyyyyyy
            args = sc.split('=>')
            from_val = args[0].strip().replace(':', '')
            to_val = args[1].strip().replace(':', '')
            if len(from_val) != len(chip.currentInputs):
                error('TEST syntax error - ', from_val)
            if len(to_val) != len(chip.currentOutputs):
                error('TEST syntax error -', to_val)

            index = 0
            list1 = ''
            list0 = ''
            for inp in chip.currentInputs:
                if from_val[index] == '1':
                    list1 = list1 + str(inp) + ','
                elif from_val[index] == '0':
                    list0 = list0 + str(inp) + ','
                else:
                    error('wrong level -', from_val[index])
                index += 1
            cmd.lst0 = str_to_int_list(list0)
            cmd.lst1 = str_to_int_list(list1)

            index = 0
            list1 = ''
            list0 = ''
            for out in chip.currentOutputs:
                if to_val[index] == '1':
                    list1 = list1 + str(out) + ','
                elif to_val[index] == '0':
                    list0 = list0 + str(out) + ','
                else:
                    error('wrong level - ', to_val[index])
                index += 1
            cmd.lst0_2 = str_to_int_list(list0)
            cmd.lst1_2 = str_to_int_list(list1)

        else:
            sc = sc.replace(':', '')
            if len(sc) == len(chip.currentOutputs):
                index = 0
                list1 = ''
                list0 = ''
                for out in chip.currentOutputs:
                    if sc[index] == '1':
                        list1 = list1 + str(out) + ','
                    elif sc[index] == '0':
                        list0 = list0 + str(out) + ','
                    else:
                        error('wrong level - ', sc[index])
                    index += 1
                cmd.lst0 = str_to_int_list(list0)
                cmd.lst1 = str_to_int_list(list1)
            else:
                error('TEST syntax error')

        chip.commands.append(cmd)

    elif line.startswith('PULSE:'):
        sc = line[len('PULSE:'):].strip()
        if sc[0] == '+':
            cmd = Command('pulse+')
        elif sc[0] == '-':
            cmd = Command('pulse-')
        else:
            error('wrong argument - ', sc)
            return

        cmd.pin = int(sc[1:].strip())

        chip.commands.append(cmd)

    elif line.startswith("CONFIG:"):
        cmd = Command('config')
        sc = line[len('CONFIG:'):].strip()
        if sc.find('->') > 0:
            lst = sc.split(';')
            for lsts in lst:
                itm = lsts.strip()
                pins = itm.split('->')[0].strip()
                direct = itm.split('->')[1].strip()
                if direct == 'IN':
                    cmd.lst0 = str_to_int_list(pins)
                elif direct == 'OUT':
                    cmd.lst1 = str_to_int_list(pins)
                else:
                    error('invalid direction ', direct)
        else:
            error('wrong syntax - ', sc)

        chip.commands.append(cmd)
        chip.currentInputs = cmd.lst0
        chip.currentOutputs = cmd.lst1

    elif line.startswith("TEST-Z"):
        cmd = Command('test-z')

        sc = line[len('TEST-Z:'):].strip()
        sc = sc.replace(':', '')
        if len(sc) == len(chip.currentOutputs):
            index = 0
            listm = ''
            for inp in chip.currentOutputs:
                if sc[index] == '1':
                    listm = listm + str(inp) + ','
                elif sc[index] == '0':
                    pass
                else:
                    error('wrong mask ', sc[index])
                index += 1
                cmd.lst1 = str_to_int_list(listm)
        else:
            error('TEST-Z syntax error')

        chip.commands.append(cmd)

    elif line.startswith("TEST-OC"):
        cmd = Command('test-oc')

        sc = line[len('TEST-OC:'):].strip()
        sc = sc.replace(':', '')
        if len(sc) == len(chip.currentOutputs):
            index = 0
            listm = ''
            for inp in chip.currentOutputs:
                if sc[index] == '1':
                    listm = listm + str(inp) + ','
                elif sc[index] == '0':
                    pass
                else:
                    error('wrong mask ', sc[index])
                index += 1
                cmd.lst1 = str_to_int_list(listm)
        else:
            error('TEST-OC syntax error')

    elif line.startswith("REPEAT-PULSE"):
        cmd = Command('repeat-pulse')
        sc = line[len('REPEAT-PULSE'):].strip()
        cmd.value = int(sc)

        chip.commands.append(cmd)

    else:
        error('wrong command - ', line)
        sys.exit(-1)


def str_to_int_list(arg):
    """
        Парсит массив чисел
    :param arg:
    :return:
    """
    result = []
    lst = arg.split(',')
    for itm in lst:
        st = itm.strip()
        if len(st) == 0:
            continue
        try:
            result.append(int(st))
        except ValueError:
            print 'ERROR: wrong numbers list: ', arg
            sys.exit(-1)
    return result


def error(msg, *args):
    """
        Выводит сообщение об ошибке и завершает работу
    :param msg:
    :param args:
    :return:
    """
    s = 'ERROR: ' + msg
    for a in args:
        s += str(a)
    print s
    sys.exit(-1)