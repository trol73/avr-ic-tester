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
def str_to_int_list(s):
    result = []
    lst = s.split(',')
    for itm in lst:
        st = itm.strip()
        if len(st) == 0:
            continue
        result.append(int(st))
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


class Command:
    name = None
    lst0 = []
    lst1 = []

    def __init__(self, name):
        self.name = name
        self.lst0 = []
        self.lst1 = []

    def show(self):
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

    def load_line(self, s):
        if s.startswith('CHIP['):
            pins = s[len('CHIP['):]
            pins = pins[:pins.find(']')]
            self.pins = int(pins)
            name = s[s.find("'"):]
            if name[0] != "'" or name[len(name) - 1] != "'":
                print 'ERROR: name expected'
                sys.exit(-1)
            self.name = name[1:-1]
        elif s.startswith('POWER:'):
            power = s[len('POWER:'):].split(' ')
            for p in power:
                if len(p) == 0:
                    continue
                if p[0] == '-':
                    self.powerMinus.append(int(p[1:]))
                elif p[0] == '+':
                    self.powerPlus.append(int(p[1:]))
        elif s.startswith('IN:'):
            inputs = s[len('IN:'):].split(',')
            for inp in inputs:
                name = inp.strip()
                if len(name) == 0:
                    continue
                self.inputs.append(int(name))
        elif s.startswith('OUT:'):
            outputs = s[len('OUT:'):].split(',')
            for out in outputs:
                name = out.strip()
                if len(name) == 0:
                    continue
                if name[0] == '@':
                    self.outputs.append(int(name[1:]))
                    self.pullUpOutputs.append(int(name[1:]))
                else:
                    self.outputs.append(int(name))
        elif s.startswith('SET:'):
            cmd = Command('set')
            lst = s[len('SET:'):].split(';')
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

            self.commands.append(cmd)

        elif s.startswith('TEST:'):
            cmd = Command('test')

            lst = s[len('TEST:'):].split(';')
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

            self.commands.append(cmd)

    def compile(self, f):
        f.write('\t')
        for ch in self.name:
            if ch != ' ':
                f.write("'")
                f.write(ch)
                f.write("', ")
        f.write("0,\n")

        f.write('\tCMD_RESET,\n')

        if self.pins <= 16:
            f.write('\tCMD_INIT_16, ')
        else:
            f.write('\tCMD_INIT_24, ')
        inputs = self.inputs
        for power in self.powerPlus:
            inputs.append(power)
        for power in self.powerMinus:
            inputs.append(power)

        f.write(self.get_pins_val(inputs) + '\n')

        f.write('\n')

        # команды
        for cmd in self.commands:
            if cmd.name == 'set':
                if self.pins <= 16:
                    f.write('\tCMD_SET_16, ')
                else:
                    f.write('\tCMD_SET_24, ')

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

            elif cmd.name == 'test':
                if self.pins <= 16:
                    f.write('\tCMD_TEST_16, ')
                else:
                    f.write('\tCMD_TEST_24, ')

                f.write(self.get_pins_val(cmd.lst0) + ' ')
                f.write(self.get_pins_val(cmd.lst1) + '\n')

        f.write('\tCMD_END,\n\n')

    # формирует битовую маску для DIP-16 и менее
    def get_pins_mask_16(self, pins):
        result = ''
        for pin in pins:
            if self.pins == 14:
                p = dip14_to_dip16(pin)
            elif self.pins == 8:
                p = dip8_to_dip16(pin)
            elif self.pins == 16:
                p = pin
            else:
                print 'ERROR: unsupported package', self.pins
                sys.exit(-1)
            if len(result) > 0:
                result += '|'
            result += '_(' + str(p) + ')'
        return result

    # формирует битовую маску для DIP-16 и менее
    def get_pins_mask_24(self, pins):
        result = ''
        for pin in pins:
            if self.pins == 20:
                p = dip20_to_dip28(pin)
            elif self.pins == 24:
                p = dip24_to_dip28(pin)
            elif self.pins == 28:
                p = pin
            else:
                print 'ERROR: unsupported package', self.pins
                sys.exit(-1)
            if len(result) > 0:
                result += '|'
            result += '_(' + str(p) + ')'
        return result

    # формирует строку маски
    def get_pins_val(self, pins):
        if self.pins <= 16:
            pins = self.get_pins_mask_16(pins)
        else:
            pins = self.get_pins_mask_24(pins)
        if self.pins <= 16:
            if len(pins) > 0:
                return 'val16(' + pins + '),'
            else:
                return 'val16(0),'
        else:
            if len(pins) > 0:
                return 'val24(' + pins + '),'
            else:
                return 'val24(0),'

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
f.write('#define val16(v)	((v) & 0xff), ((v) >> 8)\n')
f.write('\n')
f.write('#define _(v)		(1 << (v-1))\n')
f.write('\n')
f.write('const uint8_t LOGIC_DATA[] PROGMEM = {\n')

for chip in chips:
    chip.show()
    chip.compile(f)

f.write('\tCMD_END\n')
f.write('};\n')
f.close()



# CMD_RESET,
# CMD_INIT_14,	val16(_(1)|_(2)|_(4)|_(5)|_(7)|_(9)|_(10)|_(12)|_(13)|_(14)),
# CMD_SET_14,		val16(_(7)),   val16(_(14)|_(1)|_(2)|_(4)|_(5)|_(9)|_(10)|_(12)|_(13)),
# CMD_TEST_14,	val16(_(3)|_(6)|_(8)|_(11)),   val16(0),
# CMD_SET_14,		val16(_(7)|_(1)|_(2)|_(4)|_(5)|_(9)|_(10)|_(12)|_(13)),   val16(_(14)),
# CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
#
# CMD_SET_14,		val16(_(7)|_(2)|_(5)|_(10)|_(13)),   val16(_(14)|_(1)|_(4)|_(9)|_(12)),
# CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
# CMD_SET_14,		val16(_(7)|_(1)|_(4)|_(9)|_(12)),   val16(_(14)|_(2)|_(5)|_(10)|_(13)),
# CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
# CMD_END,
#
# // 155 ЛА 11, 155 ЛА 13
# 'L', 'A', '1', '1', ',', 'L', 'A', '1', '3', 0,
# CMD_RESET,
# CMD_INIT_14,	val16(_(1)|_(2)|_(4)|_(5)|_(7)|_(9)|_(10)|_(12)|_(13)|_(14)),
# CMD_SET_14,		val16(_(7)),   val16(_(14)|_(1)|_(2)|_(4)|_(5)|_(9)|_(10)|_(12)|_(13)|_(3)|_(6)|_(8)|_(11)),
# CMD_TEST_14,	val16(_(3)|_(6)|_(8)|_(11)),   val16(0),
# CMD_SET_14,		val16(_(7)|_(1)|_(2)|_(4)|_(5)|_(9)|_(10)|_(12)|_(13)),   val16(_(14)|_(3)|_(6)|_(8)|_(11)),
# CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
# CMD_SET_14,		val16(_(7)|_(2)|_(5)|_(10)|_(13)),   val16(_(14)|_(1)|_(4)|_(9)|_(12)|_(3)|_(6)|_(8)|_(11)),
# CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),
# CMD_SET_14,		val16(_(7)|_(1)|_(4)|_(9)|_(12)),   val16(_(14)|_(2)|_(5)|_(10)|_(13)|_(3)|_(6)|_(8)|_(11)),
# CMD_TEST_14,	val16(0),   val16(_(3)|_(6)|_(8)|_(11)),

