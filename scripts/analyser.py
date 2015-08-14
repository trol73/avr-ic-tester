# -*- coding: utf-8 -*-
import sys
from generator import list_to_mask

__author__ = 'trol'


class Analyser:
    """
        Анализирует выполняемые команды и собирает информацию о возможности оптимизации генерируемого кода
    """

    def __init__(self, chip_pins, chip_name):
        """

        :param chip_pins:
        :return:
        """
        self.pins = chip_pins       # количество выводов
        self.chip_name = chip_name.decode('cp1251').encode('utf8')  # название МС
        self.levels = []            # уровни ни выводах (1,0,'x')
        self.ddr = []               # направление пинов для тестера ('i', 'o')
        self.lastPulse = None       # последний импульс, поданный на МС (например, +0, -15)
        for i in range(self.pins):
            self.levels.append(0)
            self.ddr.append('i')
        # сколько выводов будет задействовано для команд
        if chip_pins <= 16:
            self.socket_pins = 16
        elif chip_pins <= 24:
            self.socket_pins = 24
        else:
            self.socket_pins = 28
        self._lastWarning = None

    def set_pins_to_0(self, mask):
        """

        :param mask:
        :return:
        """
        if isinstance(mask, (list, tuple)):
            mask = list_to_mask(mask, self.pins)

        for pin in range(self.pins):
            if mask & (1 << pin):
                self.levels[pin] = 0

    def set_pins_to_1(self, mask):
        """

        :param mask:
        :return:
        """
        if isinstance(mask, (list, tuple)):
            mask = list_to_mask(mask, self.pins)
        for pin in range(self.pins):
            if mask & (1 << pin):
                self.levels[pin] = 1

    def set_pin(self, pin, level):
        """
            Устанавливает уровень на пине тестера
        :param pin:
        :param level:
        :return:
        """
        self.levels[pin-1] = level

    def set_ddr(self, mask):
        """
            Настраивает пины тестера на ввод/вывод
        :param mask:
        :return:
        """
        if isinstance(mask, (list, tuple)):
            mask = list_to_mask(mask, self.pins)
        for pin in range(self.pins):
            if mask & (1 << pin):
                self.ddr[pin] = 'o'
            else:
                self.ddr[pin] = 'i'

    def get_levels_mask(self):
        """
            Возвращает маску для установки регистров PORT
        :return:
        """
        result = 0
        for pin in range(self.pins):
            if self.levels[pin] == 1:
                result |= 1 << pin
            elif self.levels[pin] != 0:
                print 'ERROR: pin value undefined', (pin+1), self.levels
                sys.exit(1)

        return result

    def get_test_all_mask(self, test0mask, test1mask):
        """
            Возвращает маску замены команды TEST(m0, m1) -> TEST_ALL(mask)
        :param test0mask: маска для проверки нулевых битов
        :param test1mask: маска для проверки единичных битов
        :return: маска для проверки всех пинов или None если не хватило данных для формирования маски
        """
        if isinstance(test0mask, (list, tuple)):
            test0mask = list_to_mask(test0mask, self.pins)
        if isinstance(test1mask, (list, tuple)):
            test1mask = list_to_mask(test1mask, self.pins)

        result_pins = []
        # состояние входов микросхемы должно быть таким, каким мы его установили
        for pin in range(self.pins):
            expected = None
            if (test0mask & (1 << pin)) and (test1mask & (1 << pin)):
                print 'ERROR: wrong TEST command, pin ', (pin+1)
                sys.exit(1)
            if self.ddr[pin] == 'o':
                expected = self.levels[pin]
            if expected == 0 and (test1mask & (1 << pin)):
                print 'TEST: expected output level 0 but found in mask_1 at pin ', (pin+1), ' for ', self.chip_name
            elif expected == 1 and (test0mask & (1 << pin)):
                print 'TEST: expected output level 1 but found int mask_0 at pin ', (pin+1), ' for ', self.chip_name

            if expected is None:
                if test0mask & (1 << pin):
                    expected = 0
                elif test1mask & (1 << pin):
                    expected = 1
                else:
                    self.warning("TEST: Can't detect tested level at pin ", (pin+1), ' for ', self.chip_name, '.  Unconnected pin?')
                    return None
            result_pins.append(expected)

        # проверяем, что нам известны состояния всех уровней и формируем маску
        result = 0
        for pin in range(self.pins):
            if result_pins[pin] == 1:
                result |= 1 << pin

        return result

    def pulse(self, pin, level):
        """
            Подача импульса на вывод
        :param pin: номер пина (0..pins-1)
        :param level: '-' или '+'
        :return: True, если импульс совпадает с последним подаваемым импульсом
        """
        if level == '-':
            self.set_pin(pin, 0)
        elif level == '+':
            self.set_pin(pin, 1)
        p = level + str(pin)
        if p != self.lastPulse:
            self.lastPulse = p
            return False
        return True

    def warning(self, msg, *args):
        """
            Выводит предупреждение на экран, отсеивает дубликаты
        :param msg:
        :param args:
        :return:
        """
        s = msg
        for arg in args:
            s += str(arg)
        if s != self._lastWarning:
            print s
            self._lastWarning = s
