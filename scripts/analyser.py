# -*- coding: utf-8 -*-
from generator import list_to_mask

__author__ = 'trol'


class Analyser:
    """
        Анализирует выполняемые команды и собирает информацию о возможности оптимизации генерируемого кода
    """

    def __init__(self, chip_pins):
        """

        :param chip_pins:
        :return:
        """
        self.pins = chip_pins
        self.levels = []
        self.ddr = []
        for i in range(self.pins):
            self.levels.append('x')
            self.ddr.append('o')

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

        :param pin:
        :param level:
        :return:
        """
        self.levels[pin-1] = level



