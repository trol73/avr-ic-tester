#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
from operator import attrgetter

__author__ = 'trol'


if len(sys.argv) == 2:
    src = sys.argv[1]

src = '/Users/trol/Google Drive/radio/ic-tester/Release/ic-tester.lss'


class Block:
    address = None
    name = None
    size = None
    type = None

    def __init__(self):
        self.address = None
        self.name = None
        self.size = 0
        self.type = None


blocks = []
# загружаем файл
block_in_progress = False
f = open(src, 'r')
#last_block_address = ''
#last_block_length = 0
for s in f:
    if block_in_progress:
        block = blocks[len(blocks)-1]
        if len(s.strip()) == 0:
            block_in_progress = False
#            block.size = int(last_block_address, 16) - int(block.address, 16) + last_line_length
            continue
        args = s.strip().split(':')
        if len(args) > 1:
            line_data = args[1].strip().split('\t')[0].strip().split(' ')
            if block is not None:
                if s.find('08 95') >= 0 and s.find('ret') > 0:
                    block.type = 'function'
                if s.find('f8 94') >= 0 and s.find('cli') > 0 and block.type is None:
                    block.type = 'code'
                if s.find('18 95') >= 0 and s.find('reti') > 0 and block.type is None:
                    block.type = 'code'
                if s.find('; 0x') > 0 and block.type is None:
                    block.type = 'code'
                if s.find('       	ldi	r') > 0 and block.type is None:
                    block.type = 'code'
                if s.find('       	ori	r') > 0 and block.type is None:
                    block.type = 'code'
                if s.find(' 	call	0x') > 0 and block.type is None:
                    block.type = 'code'
                if s.find('st	X+, r1') > 0 and block.type is None:
                    block.type = 'code'
                if s.find(' 00           ') > 0 and block.type is None:
                    block.type = 'string'
                if s.find('	...') >= 0 and block.type is None:
                    block.type = 'data'
                if len(args[1].split('     ')) == 2 and len(args[1].split('     ')[0]) == 48 and len(args[1].split('     ')[1]) == 16 and block.type is None:
                    block.type = 'data'

                block.size += len(args[1].strip().split('\t')[0].strip().split('  ')[0].strip().split(' '))
                # if block.name == 'Font5x7':
                #     print block.size, len(args[1].strip().split('\t')[0].strip().split('  ')[0].strip().split(' '))

                #last_block_address = args[0]

    else:
        if s.find('<') > 0 and s.find('>:') > 0:
            args = s.strip().split(' ')
            start_address = args[0]
            block_name = args[1][1:-2]
            block_in_progress = True
            block = Block()
            block.name = block_name
            block.address = start_address
            block.size = 0
            blocks.append(block)


f.close()

blocks = sorted(blocks, key=attrgetter('size'), reverse=True)

data_size = 0
code_size = 0
for block in blocks:
    if block.type == 'code' or block.type == 'function':
        code_size += block.size
        s = block.name
        while len(s) < 40:
            s += ' '
        s += str(block.size)
        print s
    else:
        data_size += block.size

print '----------------------------------\n'
print 'Code size:', code_size
print 'Data size:', data_size
#
# 00000000 <__vectors>:
#        0:	0c 94 1c 0b 	jmp	0x1638	; 0x1638 <__ctors_end>
#        4:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#        8:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#        c:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       10:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       14:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       18:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       1c:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       20:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       24:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       28:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       2c:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       30:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       34:	0c 94 c9 0b 	jmp	0x1792	; 0x1792 <__vector_13>
#       38:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       3c:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       40:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       44:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       48:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       4c:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>
#       50:	0c 94 2e 0b 	jmp	0x165c	; 0x165c <__bad_interrupt>

# 000010fc <__c.2165>:
#     10fc:	70 69 6e 73 3d 20 00                                pins= .


#
# 000011d8 <Font5x7>:
#     11d8:	00 00 00 00 00 00 00 5f 00 00 00 07 00 07 00 14     ......._........
#     11e8:	7f 14 7f 14 24 2a 7f 2a 12 23 13 08 64 62 36 49     ....$*.*.#..db6I
#     11f8:	55 22 50 00 05 03 00 00 00 1c 22 41 00 00 41 22     U"P......."A..A"
#     1208:	1c 00 08 2a 1c 2a 08 08 08 3e 08 08 00 50 30 00     ...*.*...>...P0.
#     1218:	00 08 08 08 08 08 00 60 60 00 00 20 10 08 04 02     .......``.. ....
#     1228:	3e 51 49 45 3e 00 42 7f 40 00 42 61 51 49 46 21     >QIE>.B.@.BaQIF!
#     1238:	41 45 4b 31 18 14 12 7f 10 27 45 45 45 39 3c 4a     AEK1.....'EEE9<J
#     1248:	49 49 30 01 71 09 05 03 36 49 49 49 36 06 49 49     II0.q...6III6.II
#     1258:	29 1e 00 36 36 00 00 00 56 36 00 00 00 08 14 22     )..66...V6....."
#     1268:	41 14 14 14 14 14 41 22 14 08 00 02 01 51 09 06     A.....A".....Q..
#     1278:	32 49 79 41 3e 7e 11 11 11 7e 7f 49 49 49 36 3e     2IyA>~...~.III6>
#     1288:	41 41 41 22 7f 41 41 22 1c 7f 49 49 49 41 7f 09     AAA".AA"..IIIA..
#     1298:	09 01 01 3e 41 41 51 32 7f 08 08 08 7f 00 41 7f     ...>AAQ2......A.
#     12a8:	41 00 20 40 41 3f 01 7f 08 14 22 41 7f 40 40 40     A. @A?...."A.@@@
#     12b8:	40 7f 02 04 02 7f 7f 04 08 10 7f 3e 41 41 41 3e     @..........>AAA>
#     12c8:	7f 09 09 09 06 3e 41 51 21 5e 7f 09 19 29 46 46     .....>AQ!^...)FF
#     12d8:	49 49 49 31 01 01 7f 01 01 3f 40 40 40 3f 1f 20     III1.....?@@@?.
#     12e8:	40 20 1f 7f 20 18 20 7f 63 14 08 14 63 03 04 78     @ .. . .c...c..x
#     12f8:	04 03 61 51 49 45 43 00 00 7f 41 41 02 04 08 10     ..aQIEC...AA....
#     1308:	20 41 41 7f 00 00 04 02 01 02 04 40 40 40 40 40      AA........@@@@@
#     1318:	00 01 02 04 00 20 54 54 54 78 7f 48 44 44 38 38     ..... TTTx.HDD88
#     1328:	44 44 44 20 38 44 44 48 7f 38 54 54 54 18 08 7e     DDD 8DDH.8TTT..~
#     1338:	09 01 02 08 14 54 54 3c 7f 08 04 04 78 00 44 7d     .....TT<....x.D}
#     1348:	40 00 20 40 44 3d 00 00 7f 10 28 44 00 41 7f 40     @. @D=....(D.A.@
#     1358:	00 7c 04 18 04 78 7c 08 04 04 78 38 44 44 44 38     .|...x|...x8DDD8
#     1368:	7c 14 14 14 08 08 14 14 18 7c 7c 08 04 04 08 48     |........||....H
#     1378:	54 54 54 20 04 3f 44 40 20 3c 40 40 20 7c 1c 20     TTT .?D@ <@@ |.
#     1388:	40 20 1c 3c 40 30 40 3c 44 28 10 28 44 0c 50 50     @ .<@0@<D(.(D.PP
#     1398:	50 3c 44 64 54 4c 44 00 08 36 41 00 00 00 7f 00     P<DdTLD..6A.....
#     13a8:	00 00 41 36 08 00 08 08 2a 1c 08 08 1c 2a 08 08     ..A6....*....*..
#     13b8:	14 36 77 36 14 00 7f 3e 1c 08 08 1c 3e 7f 00 04     .6w6...>....>...
#     13c8:	06 07 06 04 10 30 70 30 10 3e 41 08 14 14 41 3e     .....0p0.>A...A>
#     13d8:	00 00 00 10 38 7c 10 1f 08 1c 3e 08 08 00 00 00     ....8|....>.....
# 	...
#     14f8:	7e 11 11 11 7e 7f 49 49 49 33 7f 49 49 49 36 7f     ~...~.III3.III6.
#     1508:	01 01 01 03 e0 51 4f 41 ff 7f 49 49 49 41 77 08     .....QOA..IIIAw.
#     1518:	7f 08 77 41 49 49 49 36 7f 10 08 04 7f 7c 21 12     ..wAIII6.....|!.
#     1528:	09 7c 7f 08 14 22 41 20 41 3f 01 7f 7f 02 0c 02     .|..."A A?......
#     1538:	7f 7f 08 08 08 7f 3e 41 41 41 3e 7f 01 01 01 7f     ......>AAA>.....
#     1548:	7f 09 09 09 06 3e 41 41 41 22 01 01 7f 01 01 47     .....>AAA".....G
#     1558:	28 10 08 07 1c 22 7f 22 1c 63 14 08 14 63 7f 40     (....".".c...c.@
#     1568:	40 40 ff 07 08 08 08 7f 7f 40 7f 40 7f 7f 40 7f     @@.......@.@..@.
#     1578:	40 ff 01 7f 48 48 30 7f 48 30 00 7f 00 7f 48 48     @...HH0.H0....HH
#     1588:	30 22 41 49 49 3e 7f 08 3e 41 3e 46 29 19 09 7f     0"AII>..>A>F)...
#     1598:	20 54 54 54 78 3c 4a 4a 49 31 7c 54 54 28 00 7c      TTTx<JJI1|TT(.|
#     15a8:	04 04 04 0c e0 54 4c 44 fc 38 54 54 54 18 6c 10     .....TLD.8TTT.l.
#     15b8:	7c 10 6c 44 44 54 54 28 7c 20 10 08 7c 7c 41 22     |.lDDTT(| ..||A"
#     15c8:	11 7c 7c 10 28 44 00 20 44 3c 04 7c 7c 08 10 08     .||.(D. D<.||...
#     15d8:	7c 7c 10 10 10 7c 38 44 44 44 38 7c 04 04 04 7c     ||...|8DDD8|...|
#     15e8:	7c 14 14 14 08 38 44 44 44 20 04 04 7c 04 04 0c     |....8DDD ..|...
#     15f8:	50 50 50 3c 30 48 fc 48 30 44 28 10 28 44 7c 40     PPP<0H.H0D(.(D|@
#     1608:	40 40 fc 0c 10 10 10 7c 7c 40 7c 40 7c 7c 40 7c     @@.....||@|@||@|
#     1618:	40 fc 04 7c 50 50 20 7c 50 50 20 7c 7c 50 50 20     @..|PP |PP ||PP
#     1628:	00 28 44 54 54 38 7c 10 38 44 38 08 54 34 14 7c     .(DTT8|.8D8.T4.|


# 00001674 <uart_putc_hex>:
#     1674:	cf 93       	push	r28
#     1676:	c8 2f       	mov	r28, r24
#     1678:	82 95       	swap	r24
#     167a:	8f 70       	andi	r24, 0x0F	; 15
#     167c:	8a 30       	cpi	r24, 0x0A	; 10
#     167e:	10 f4       	brcc	.+4      	; 0x1684 <uart_putc_hex+0x10>
#     1680:	80 5d       	subi	r24, 0xD0	; 208
#     1682:	01 c0       	rjmp	.+2      	; 0x1686 <uart_putc_hex+0x12>
#     1684:	89 5a       	subi	r24, 0xA9	; 169
#     1686:	0e 94 36 0b 	call	0x166c	; 0x166c <uart_putc>
#     168a:	8c 2f       	mov	r24, r28
#     168c:	8f 70       	andi	r24, 0x0F	; 15
#     168e:	28 2f       	mov	r18, r24
#     1690:	30 e0       	ldi	r19, 0x00	; 0
#     1692:	2a 30       	cpi	r18, 0x0A	; 10
#     1694:	31 05       	cpc	r19, r1
#     1696:	14 f4       	brge	.+4      	; 0x169c <uart_putc_hex+0x28>
#     1698:	80 5d       	subi	r24, 0xD0	; 208
#     169a:	01 c0       	rjmp	.+2      	; 0x169e <uart_putc_hex+0x2a>
#     169c:	89 5a       	subi	r24, 0xA9	; 169
#     169e:	cf 91       	pop	r28
#     16a0:	0c 94 36 0b 	jmp	0x166c	; 0x166c <uart_putc>
