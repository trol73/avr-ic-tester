# -*- coding: utf-8 -*-

__author__ = 'trol'


class Command:
    # name = None
    # lst0 = []
    # lst1 = []
    # lst0_2 = []
    # lst1_2 = []
    # pin = None

    def __init__(self, name):
        self.name = name
        self.lst0 = []
        self.lst1 = []
        self.lst0_2 = []
        self.lst1_2 = []
        self.pin = None

    def show(self):
        if len(self.lst0_2) + len(self.lst1_2) > 0:
            print '#', self.name, self.lst0, self.lst1, ' => ', self.lst0_2, self.lst1_2
        else:
            print '#', self.name, self.lst0, self.lst1


class Chip:
    # pins = None  # количество выводов DIP
    # name = None  # имя микросхем(ы)
    # powerPlus = []  # номер вывода питания
    # powerMinus = []  # номер вывода земли
    # inputs = []  # номера входов
    # outputs = []  # номера выходов
    # pullUpOutputs = []
    # commands = []
    # currentInputs = []
    # currentOutputs = []

    def __init__(self):
        self.pins = None
        self.name = None
        self.powerMinus = []
        self.powerPlus = []
        self.inputs = []
        self.outputs = []
        self.pullUpOutputs = []
        self.commands = []
        self.currentInputs = []
        self.currentOutputs = []

    def show(self):
        print 'Name:', self.name
        print 'DIP-' + str(self.pins)
        print 'power: -' + str(self.powerMinus) + ' +' + str(self.powerPlus)
        print 'inputs:', self.inputs
        print 'outputs', self.outputs
        print 'pull-up', self.pullUpOutputs
        for cmd in self.commands:
            cmd.show()