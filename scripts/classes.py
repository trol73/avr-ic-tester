# -*- coding: utf-8 -*-

__author__ = 'trol'


class Command:

    def __init__(self, name):
        self.name = name            # название команды (внутреннее, для компилятора)
        self.lst0 = []              # список для маски нулевых битов (команды SET и TEST)
        self.lst1 = []              # список для маски единичных битов (команды SET и TEST)
        self.lst0_2 = []            # список для маски нулевых битов (команда SET + TEST)
        self.lst1_2 = []            # список для маски единичных битов (команда SET + TEST)
        self.pin = None             # номер пина для команд PULSE

    def show(self):
        if len(self.lst0_2) + len(self.lst1_2) > 0:
            print '#', self.name, self.lst0, self.lst1, ' => ', self.lst0_2, self.lst1_2
        else:
            print '#', self.name, self.lst0, self.lst1


class Chip:
    # pins = None
    # name = None
    # powerPlus = []  # номер вывода питания
    # powerMinus = []  # номер вывода земли
    # inputs = []  # номера входов
    # outputs = []  # номера выходов
    # pullUpOutputs = []
    # commands = []
    # currentInputs = []
    # currentOutputs = []

    def __init__(self):
        self.pins = None            # количество выводов DIP
        self.name = None            # имя микросхем(ы)
        self.powerMinus = []        # номера выводов питания -
        self.powerPlus = []         # номера выводов питания +
        self.inputs = []            # список входов микросхемы
        self.outputs = []           # список выходов микросхемы
        self.pullUpOutputs = []     # список выходов микросхемы, для которых нужна подтяжка к питанию (выходы с открытым коллектором)
        self.commands = []          # список команд
        self.currentInputs = []     # список входов микросхемы с учетомы переконфигураций (если пины работают как на вход, так на выход)
        self.currentOutputs = []    # список выходов микросхемы с учетомы переконфигураций (если пины работают как на вход, так на выход)
        self.voidPins = []          # список выводов микросхемы, которые никуда не подключены

    def show(self):
        print 'Name:', self.name
        print 'DIP-' + str(self.pins)
        print 'power: -' + str(self.powerMinus) + ' +' + str(self.powerPlus)
        print 'inputs:', self.inputs
        print 'outputs', self.outputs
        print 'pull-up', self.pullUpOutputs
        for cmd in self.commands:
            cmd.show()