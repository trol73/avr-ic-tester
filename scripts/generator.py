# -*- coding: utf-8 -*-
import sys

__author__ = 'trol'


def list_to_mask(mask, pins):
    """
        Перобразует список пинов в битовую маску
    :param mask:
    :param pins:
    :return:
    """
    res = 0
    for pin in mask:
        if pin <= 0 or pin > pins:
            print 'ERROR: invalid pin number for DIP-' + str(pins) + ':', pin
            sys.exit(-1)
        res |= 1 << (pin-1)
    return res


def convert_bitmask(mask, from_pins, to_pins, set_unused_bits_to=0):
    """
        Преобразует битовую маску
    :param mask:
    :param from_pins:
    :param to_pins:
    :param set_unused_bits_to: каким значением инициализировать неиспользуемые биты (например, если МС DIP-14 вставляется в панель DIP-16
    :return:
    """
    result = 0
    for src_pin in range(from_pins):
        out_pin = src_pin
        if out_pin >= from_pins/2:
            out_pin += to_pins - from_pins
        if mask & (1 << src_pin):
            result |= (1 << out_pin)
    # если неиспользуемые пины надо установить в 1
    if set_unused_bits_to == 1:
        for pin in range(from_pins/2, to_pins/2):
            result |= 1 << pin
            result |= 1 << (pin + (to_pins - from_pins)/2)

    return result


def convert_pin(pin, from_pins, to_pins):
    """
        Преобразует номер пина
    :param pin: номер пина (нумерация от 1)
    :param from_pins:
    :param to_pins:
    :return:
    """
    if pin <= from_pins/2:
        return pin
    else:
        return pin + to_pins - from_pins


def binary_byte(b):
    """
        Возвращает строку с двоичным представлением байта в Си
    :param b:
    :return:
    """
    result = bin(b)
    while len(result) < 10:
        result = result.replace('0b', '0b0')
    return result


class DataGenerator:

    def __init__(self):
        self.commands = []
        self.size = 0

    def add_chip(self, name):
        compiled_name = ''
        for ch in name:
            if ch != ' ':
                compiled_name += ch
        self.commands.append(name)

    def add_command(self, *args):
        """

        :param args:
        :return:
        """
        cmd = []
        for a in args:
            cmd.append(a)
        self.commands.append(cmd)
        self.size += len(args)

    def add_command_mask_1(self, name, mask, pins, set_unused_bits_to=0):
        """
            Добавляет команду с аргументом-маской

        :param name: название команды без суффикса размера (к немму будет добавлено _16, _24 или _28) в зависимости от числа пинов МС
        :param mask: значение маски, число либо массив с номерами установленных битов
        :param pins: количество пинов у МС
        :param set_unused_bits_to: каким значением инициализировать неиспользуемые биты (например, если МС DIP-14 вставляется в панель DIP-16)
        :return:
        """
        if isinstance(mask, (list, tuple)):
            mask = list_to_mask(mask, pins)
        if pins <= 16:
            mask = convert_bitmask(mask, pins, 16, set_unused_bits_to)
            self.add_command(name + '_16', binary_byte(mask & 0xff), binary_byte((mask >> 8) & 0xff))
        elif pins <= 24:
            mask = convert_bitmask(mask, pins, 24, set_unused_bits_to)
            self.add_command(name + '_24', binary_byte(mask & 0xff), binary_byte((mask >> 8) & 0xff), binary_byte((mask >> 16) & 0xff))
        else:
            mask = convert_bitmask(mask, pins, 28, set_unused_bits_to)
            self.add_command(name + '_28', binary_byte(mask & 0xff), binary_byte((mask >> 8) & 0xff), binary_byte((mask >> 16) & 0xff),
                             binary_byte((mask >> 24) & 0xff))

    def add_command_mask_2(self, name, mask1, mask2, pins, set_unused_bits_to=0):
        """
            Добавляет команду с двумя аргументами-масками

        :param name: название команды без суффикса размера (к немму будет добавлено _16, _24 или _28) в зависимости от числа пинов МС
        :param mask1: значение маски первого аргумента, число либо массив с номерами установленных битов
        :param mask2: значение маски второго аргумента, число либо массив с номерами установленных битов
        :param pins: количество пинов у МС
        :param set_unused_bits_to: каким значением инициализировать неиспользуемые биты (например, если МС DIP-14 вставляется в панель DIP-16)
        :return:
        """
        if isinstance(mask1, (list, tuple)):
            mask1 = list_to_mask(mask1, pins)
        if isinstance(mask2, (list, tuple)):
            mask2 = list_to_mask(mask2, pins)

        if pins <= 16:
            mask1 = convert_bitmask(mask1, pins, 16, set_unused_bits_to)
            mask2 = convert_bitmask(mask2, pins, 16, set_unused_bits_to)
            self.add_command(name + '_16', binary_byte(mask1 & 0xff), binary_byte((mask1 >> 8) & 0xff),
                             binary_byte(mask2 & 0xff), binary_byte((mask2 >> 8) & 0xff))
        elif pins <= 24:
            mask1 = convert_bitmask(mask1, pins, 24, set_unused_bits_to)
            mask2 = convert_bitmask(mask2, pins, 24, set_unused_bits_to)
            self.add_command(name + '_24', binary_byte(mask1 & 0xff), binary_byte((mask1 >> 8) & 0xff), binary_byte((mask1 >> 16) & 0xff),
                             binary_byte(mask2 & 0xff), binary_byte((mask2 >> 8) & 0xff), binary_byte((mask2 >> 16) & 0xff))
        else:
            mask1 = convert_bitmask(mask1, pins, 28, set_unused_bits_to)
            mask2 = convert_bitmask(mask2, pins, 28, set_unused_bits_to)
            self.add_command(name + '_28', binary_byte(mask1 & 0xff), binary_byte((mask1 >> 8) & 0xff), binary_byte((mask1 >> 16) & 0xff),
                             binary_byte((mask1 >> 24) & 0xff), binary_byte(mask2 & 0xff), binary_byte((mask2 >> 8) & 0xff),
                             binary_byte((mask2 >> 16) & 0xff), binary_byte((mask2 >> 24) & 0xff))

    def generate(self, filename):
        """
            Сохраняет результат генерации в файл

        :param filename:
        :return:
        """
        f = open(filename, 'w')
        f.write('\n')
        f.write('const uint8_t LOGIC_DATA[] PROGMEM = {')

        for cmd in self.commands:
            first = True
            if isinstance(cmd, (list, tuple)):
                for b in cmd:
                    if first:
                        f.write('\t')
                        first = False
                    else:
                        f.write(' ')
                    f.write(str(b))
                    f.write(',')
                f.write('\n')
            else:
                f.write('\n')
                f.write('\t')
                for ch in cmd:
                    if ch != ' ':
                        f.write("'")
                        f.write(ch)
                        f.write("', ")
                        self.size += 1
                f.write("0,\n")
                self.size += 1

        f.write('\n')

        f.write('\tCMD_END\n')
        self.size += 1
        f.write('};\n')
        f.close()

