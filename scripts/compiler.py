#!/usr/bin/python
# -*- coding: utf-8 -*-
import sys

__author__ = 'trol'

src = 'data.ic'
out = '../ic-tester/data.h'

if len(sys.argv) == 2:
    src = sys.argv[1]

print 'compile', src, 'to', out


# парсит массив чисел
def str_to_int_list(arg):
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


# переводит пин из DIP-8 в DIP-16
def dip8_to_dip16(pin):
    if pin <= 4:
        return pin
    if pin <= 8:
        return pin + 8
    print 'ERROR: invalid pin number', pin, 'for DIP-8'
    sys.exit(-1)


# переводит пин из DIP-14 в DIP-16
def dip14_to_dip16(pin):
    if pin <= 7:
        return pin
    if pin <= 14:
        return pin + 2
    print 'ERROR: invalid pin number', pin, 'for DIP-14'
    sys.exit(-1)


def dip8_to_dip28(pin):
    if pin <= 4:
        return pin
    if pin <= 8:
        return pin + 20
    print 'ERROR: invalid pin number', pin, 'for DIP-8'
    sys.exit(-1)


def dip14_to_dip28(pin):
    if pin <= 7:
        return pin
    if pin <= 14:
        return pin + 14
    print 'ERROR: invalid pin number', pin, 'for DIP-14'
    sys.exit(-1)


def dip16_to_dip28(pin):
    if pin <= 8:
        return pin
    if pin <= 16:
        return pin + 12
    print 'ERROR: invalid pin number', pin, 'for DIP-16'
    sys.exit(-1)


def dip20_to_dip28(pin):
    if pin <= 10:
        return pin
    if pin <= 20:
        return pin + 8
    print 'ERROR: invalid pin number', pin, 'for DIP-20'
    sys.exit(-1)


def dip24_to_dip28(pin):
    if pin <= 12:
        return pin
    if pin <= 24:
        return pin + 4
    print 'ERROR: invalid pin number', pin, 'for DIP-24'
    sys.exit(-1)


def dip20_to_dip24(pin):
    if pin <= 10:
        return pin
    if pin <= 20:
        return pin + 4
    print 'ERROR: invalid pin number', pin, 'for DIP-20'
    sys.exit(-1)


def binary_byte(b):
    result = bin(b)
    while len(result) < 10:
        result = result.replace('0b', '0b0')
    return result


class Command:
    name = None
    lst0 = []
    lst1 = []
    lst0_2 = []
    lst1_2 = []
    pin = None

    def __init__(self, name):
        self.name = name
        self.lst0 = []
        self.lst1 = []
        self.lst0_2 = []
        self.lst1_2 = []
        pin = None

    def show(self):
        if len(self.lst0_2) + len(self.lst1_2) > 0:
            print '#', self.name, self.lst0, self.lst1, ' => ', self.lst0_2, self.lst1_2
        else:
            print '#', self.name, self.lst0, self.lst1


class Chip:
    pins = None  # количество выводов DIP
    name = None  # имя микросхем(ы)
    powerPlus = []  # номер вывода питания
    powerMinus = []  # номер вывода земли
    inputs = []  # номера входов
    outputs = []  # номера выходов
    pullUpOutputs = []
    commands = []

    def __init__(self):
        self.powerMinus = []
        self.powerPlus = []
        self.inputs = []
        self.outputs = []
        self.pullUpOutputs = []
        self.commands = []

    def load_line(self, line):
        if line.startswith('CHIP['):
            pins = line[len('CHIP['):]
            pins = pins[:pins.find(']')]
            self.pins = int(pins)
            name = line[line.find("'"):]
            if name[0] != "'" or name[len(name) - 1] != "'":
                print 'ERROR: name expected'
                sys.exit(-1)
            self.name = name[1:-1]
        elif line.startswith('POWER:'):
            power = line[len('POWER:'):].split(' ')
            for p in power:
                if len(p.strip()) == 0:
                    continue
                if p[0] == '-':
                    self.powerMinus.append(int(p[1:]))
                elif p[0] == '+':
                    self.powerPlus.append(int(p[1:]))
        elif line.startswith('IN:'):
            inputs = line[len('IN:'):].split(',')
            for inp in inputs:
                name = inp.strip()
                if len(name) == 0:
                    continue
                n = int(name)
                if n > self.pins:
                    print 'ERROR: wrong pin number', n, 'for DIP-'+str(self.pins)
                    sys.exit(-1)
                self.inputs.append(n)
        elif line.startswith('OUT:'):
            outputs = line[len('OUT:'):].split(',')
            for out in outputs:
                name = out.strip()
                if len(name) == 0:
                    continue
                if name[0] == '@':
                    n = int(name[1:])
                    if n > self.pins:
                        print 'ERROR: wrong pin number', n, 'for DIP-'+str(self.pins)
                        sys.exit(-1)
                    self.outputs.append(n)
                    self.pullUpOutputs.append(n)
                else:
                    n = int(name)
                    if n > self.pins:
                        print 'ERROR: wrong pin number', n, 'for DIP-'+str(self.pins)
                        sys.exit(-1)
                    self.outputs.append(n)
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
                        print 'ERROR: invalid level ' + level
                        sys.exit(-1)
            else:
                sc = sc.replace(':', '')
                if len(sc) == len(self.inputs):
                    index = 0
                    list1 = ''
                    list0 = ''
                    for inp in self.inputs:
                        if sc[index] == '1':
                            list1 = list1 + str(inp) + ','
                        elif sc[index] == '0':
                            list0 = list0 + str(inp) + ','
                        else:
                            print 'ERROR: wrong level', sc[index]
                            sys.exit(-1)
                        index += 1
                    cmd.lst0 = str_to_int_list(list0)
                    cmd.lst1 = str_to_int_list(list1)
                else:
                    print 'ERROR: SET syntax error'
                    sys.exit(-1)

            self.commands.append(cmd)

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
                        print 'ERROR: invalid level ' + level
                        sys.exit(-1)
            elif sc.find('=>') > 0:
                cmd.name = 'set+test'
                # команда вида TEST: xxxx => yyyyyyyy
                args = sc.split('=>')
                from_val = args[0].strip().replace(':', '')
                to_val = args[1].strip().replace(':', '')
                if len(from_val) != len(self.inputs):
                    print 'ERROR: TEST syntax error:', from_val
                    sys.exit(-1)
                if len(to_val) != len(self.outputs):
                    print 'ERROR: TEST syntax error:', to_val
                    sys.exit(-1)

                index = 0
                list1 = ''
                list0 = ''
                for inp in self.inputs:
                    if from_val[index] == '1':
                        list1 = list1 + str(inp) + ','
                    elif from_val[index] == '0':
                        list0 = list0 + str(inp) + ','
                    else:
                        print 'ERROR: wrong level', from_val[index]
                        sys.exit(-1)
                    index += 1
                cmd.lst0 = str_to_int_list(list0)
                cmd.lst1 = str_to_int_list(list1)

                index = 0
                list1 = ''
                list0 = ''
                for out in self.outputs:
                    if to_val[index] == '1':
                        list1 = list1 + str(out) + ','
                    elif to_val[index] == '0':
                        list0 = list0 + str(out) + ','
                    else:
                        print 'ERROR: wrong level', to_val[index]
                        sys.exit(-1)
                    index += 1
                cmd.lst0_2 = str_to_int_list(list0)
                cmd.lst1_2 = str_to_int_list(list1)

            else:
                sc = sc.replace(':', '')
                if len(sc) == len(self.outputs):
                    index = 0
                    list1 = ''
                    list0 = ''
                    for out in self.outputs:
                        if sc[index] == '1':
                            list1 = list1 + str(out) + ','
                        elif sc[index] == '0':
                            list0 = list0 + str(out) + ','
                        else:
                            print 'ERROR: wrong level', sc[index]
                            sys.exit(-1)
                        index += 1
                    cmd.lst0 = str_to_int_list(list0)
                    cmd.lst1 = str_to_int_list(list1)
                else:
                    print 'ERROR: TEST syntax error'
                    sys.exit(-1)

            self.commands.append(cmd)
        elif line.startswith('PULSE:'):
            sc = line[len('PULSE:'):].strip()
            cmd = None
            if sc[0] == '+':
                cmd = Command('pulse+')
            elif sc[0] == '-':
                cmd = Command('pulse-')
            else:
                print 'ERROR: wrong argument - ', sc

            cmd.pin = int(sc[1:].strip())

            self.commands.append(cmd)
        else:
            print 'ERROR: wrong command', line
            sys.exit(-1)

    def compile(self, f):
        """

        :param f:
        """
        size = 0

        f.write('\t')
        for ch in self.name:
            if ch != ' ':
                f.write("'")
                f.write(ch)
                f.write("', ")
                size += 1
        f.write("0,\n")
        size += 1

        f.write('\tCMD_RESET,\n')
        size += 1

        if self.pins <= 16:
            f.write('\tCMD_INIT_16, ')
            size += 3
        elif self.pins <= 24:
            f.write('\tCMD_INIT_24, ')
            size += 4
        else:
            f.write('\tCMD_INIT_28, ')
            size += 5
        inputs = self.inputs
        for power in self.powerPlus:
            inputs.append(power)
        for power in self.powerMinus:
            inputs.append(power)

        f.write(self.get_pins_val(inputs) + '\t// ' + self.get_pins_comment(inputs) + '\n')

        f.write('\n')

        # команды
        for cmd in self.commands:
            if cmd.name == 'set':
                if self.pins <= 16:
                    f.write('\tCMD_SET_16, ')
                    size += 5
                elif self.pins <= 24:
                    f.write('\tCMD_SET_24, ')
                    size += 6
                else:
                    f.write('\tCMD_SET_28, ')
                    size += 7

                pins0 = cmd.lst0
                for power in self.powerMinus:
                    pins0.append(power)
                f.write(self.get_pins_val(pins0) + ' ')

                pins1 = cmd.lst1
                for power in self.powerPlus:
                    pins1.append(power)
                for pullUp in self.pullUpOutputs:
                    pins1.append(pullUp)
                f.write(self.get_pins_val(pins1) + '\t// ' + self.get_pins_comment(pins0) + '\t' + self.get_pins_comment(pins1) + '\n')

            elif cmd.name == 'test':
                if self.pins <= 16:
                    f.write('\tCMD_TEST_16, ')
                    size += 5
                elif self.pins <= 24:
                    f.write('\tCMD_TEST_24, ')
                    size += 6
                else:
                    f.write('\tCMD_TEST_28, ')
                    size += 7

                f.write(self.get_pins_val(cmd.lst0) + ' ')
                f.write(self.get_pins_val(cmd.lst1) + '\n')

            elif cmd.name == 'set+test':
                if self.pins <= 16:
                    f.write('\tCMD_SET_16, ')
                    size += 5
                elif self.pins <= 24:
                    f.write('\tCMD_SET_24, ')
                    size += 6
                else:
                    f.write('\tCMD_SET_28, ')
                    size += 7

                pins0 = cmd.lst0
                for power in self.powerMinus:
                    pins0.append(power)
                f.write(self.get_pins_val(pins0) + ' ')

                pins1 = cmd.lst1
                for power in self.powerPlus:
                    pins1.append(power)
                for pullUp in self.pullUpOutputs:
                    pins1.append(pullUp)
                f.write(self.get_pins_val(pins1) + '\n')

                if self.pins <= 16:
                    f.write('\tCMD_TEST_16, ')
                    size += 5
                elif self.pins <= 24:
                    f.write('\tCMD_TEST_24, ')
                    size += 6
                else:
                    f.write('\tCMD_TEST_28, ')
                    size += 7

                f.write(self.get_pins_val(cmd.lst0_2) + ' ')
                f.write(self.get_pins_val(cmd.lst1_2) + '\n')

            elif cmd.name == 'pulse+':
                f.write('\tCMD_PULSE_PLUS, ')
                f.write(str(self.get_dip28_num(cmd.pin)) + ',\n')
                size += 2

            elif cmd.name == 'pulse-':
                f.write('\tCMD_PULSE_MINUS, ')
                f.write(str(self.get_dip28_num(cmd.pin)) + ',\n')
                size += 2

        f.write('\tCMD_END,\n\n')
        size += 1
        return size

    # формирует битовую маску
    def get_pins_mask(self, pins):
        result = 0
        for pin in pins:
            if self.pins == 8:
                p = dip8_to_dip16(pin)
            elif self.pins == 14:
                p = dip14_to_dip16(pin)
            elif self.pins == 16:
                p = pin
            elif self.pins == 20:
                p = dip20_to_dip24(pin)
            elif self.pins == 24:
                p = pin
            elif self.pins == 28:
                p = pin
            else:
                print 'ERROR: unsupported package', self.pins
                sys.exit(-1)
            result |= 1 << (p-1)
        return result

    #
    # def get_pins_value_28(self, pins):
    #     result = 0
    #     for pin in pins:
    #         if self.pins == 20:
    #             p = dip20_to_dip28(pin)
    #         elif self.pins == 24:
    #             p = dip24_to_dip28(pin)
    #         elif self.pins == 28:
    #             p = pin
    #         else:
    #             print 'ERROR: unsupported package DIP-', self.pins
    #             sys.exit(-1)
    #         result |= 1 << (pin-1)
    #     return result

    # преобразует номер пина МС в номер для DIP-28
    def get_dip28_num(self, pin):
        if self.pins == 8:
            return dip8_to_dip28(pin)
        elif self.pins == 14:
            return dip14_to_dip28(pin)
        elif self.pins == 16:
            return dip16_to_dip28(pin)
        elif self.pins == 20:
            return dip20_to_dip28(pin)
        elif self.pins == 28:
            return pin
        else:
            print 'ERROR: unsupported package DIP-', self.pins
            sys.exit(-1)

    # формирует строку маски
    def get_pins_val(self, pins):
        v = self.get_pins_mask(pins)
        result = binary_byte(v & 0xff) + ', ' + binary_byte((v >> 8) & 0xff) + ', '
        if self.pins > 16:
            result += binary_byte((v >> 16) & 0xff) + ', '
        if self.pins > 24:
            result += binary_byte((v >> 24) & 0xff) + ', '
        return result

    def get_pins_comment(self, pins):
        v = self.get_pins_mask(pins)
        result = ''
        for i in range(0, 30):
            if (v & (1 << i)) != 0:
                result += str(i+1) + ' '
        return result + '     ' + str(pins)

    def show(self):
        print 'Name:', self.name
        print 'DIP-' + str(self.pins)
        print 'power: -' + str(self.powerMinus) + ' +' + str(self.powerPlus)
        print 'inputs:', self.inputs
        print 'outputs', self.outputs
        print 'pull-up', self.pullUpOutputs
        for cmd in self.commands:
            cmd.show()


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
    print s

    if s.startswith('CHIP['):
        chip = Chip()
        chip.load_line(s)
        chips.append(chip)
    else:
        chips[len(chips) - 1].load_line(s)

f.close()

f = open(out, 'w')
f.write('\n')
f.write('const uint8_t LOGIC_DATA[] PROGMEM = {\n')

size = 0
for chip in chips:
    chip.show()
    size += chip.compile(f)

f.write('\tCMD_END\n')
size += 1
f.write('};\n')
f.close()


print 'Total chips: ', len(chips)
print 'Data size: ', size

