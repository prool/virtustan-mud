#35700
у ваньки~
0 r 100
~
if %actor.clan%==сп
wait 5
say За скатертью пришли?
say Она будет стоить Вам 100 кун.
else
say Чужим здесь не место!
end
~
#35701
дали куны ваньке~
0 m 100
100~
if %actor.clan%==сп
tell %actor.name% Хорошо!
if %world.curobjs(35710)% < 50
tell %actor.name% Держи свою скатерть!
mload obj 35727
give скатерть %actor.name%
else
tell %actor.name% Все, нету скатеретей.
end
else
say Чужим здесь не место!
end
~
#35702
пурж стаффа~
1 g 100
~
wait 1
if %actor.clan% == сп
  halt
endif
%purge% %self%
~
#35703
обкаст~
0 m 0
~
wait 1
фиг %actor.name%
halt
if %amount% < 20
msend %actor.name% За такие деньги я даже пальцем не пошевелю!
halt
endif
switch %amount%
case 20
mecho Священик зачитал свиток.
dg_cast 'критич исцеление' %actor.name%
break
case 30
mecho Священик прочитал заклинание из книги.
dg_cast 'доблесть' %actor.name%
break
case 40
mecho Священик посмотрел в резы и что-то пробормотал.
dg_cast 'невидимость' %actor.name%
break
case 50
mecho Священик зачитал свиток.
dg_cast 'защита' %actor.name%
break
case 60
mecho Священик зачитал свиток.
dg_cast 'затуманивание' %actor.name%
break
case 70
mecho Священик скукожился аки гриб сухой.
dg_cast 'сила' %actor.name%
break
case 80
mecho Священик взмахнул руками.
dg_cast 'полет' %actor.name%
break
case 90
mecho Священик взмахнул руками.
dg_cast 'быст восст' %actor.name%
break
case 350
mecho Священик зачитал свиток
dg_cast 'доблесть' %actor.name%
dg_cast 'невидимость' %actor.name%
dg_cast 'защита' %actor.name%
dg_cast 'затуманивание' %actor.name%
dg_cast 'сила' %actor.name%
dg_cast 'полет' %actor.name%
dg_cast 'быст восст' %actor.name%
break
default
msend %actor% Извините, но я не знаю что Вы хотите от меня за эти деньги.
msend %actor% Вот что я могу сделать для Вас:
msend %actor%  за 20 кун - подлечу тебя чуток.
msend %actor%  за 30 кун - Бог будет с тобой.
msend %actor%  за 40 кун - невидимкою будешь.
msend %actor%  за 50 кун - ты станешь более защищен.
msend %actor%  за 60 кун - твое тело станет туманным.
msend %actor%  за 70 кун - сильнее во стократ станешь.
msend %actor%  за 80 кун - сможешь летать,аки сокол в небе.
msend %actor%  за 90 кун - сможешь быстрее жизнь и энергию восстанавливать.
msend %actor%  за 350 кун - могу сразу все накастить...
done
~
#35704
приход к знахарю~
0 g 100
~
wait 1
msend %actor% Вот что я могу сделать для Вас:
msend %actor%  за 20 кун - подлечу тебя чуток.
msend %actor%  за 30 кун - Бог будет с тобой.
msend %actor%  за 40 кун - невидимкою будешь.
msend %actor%  за 50 кун - ты станешь более защищен.
msend %actor%  за 60 кун - твое тело станет туманным.
msend %actor%  за 70 кун - сильнее во стократ станешь.
msend %actor%  за 80 кун - сможешь летать,аки сокол в небе.
msend %actor%  за 80 кун - сможешь быстрее жизнь и энергию восстанавливать.
msend %actor%  за 350 кун - могу сразу все накастить...
end
~
#35705
развернуть скатерть~
1 c 3
развернуть~
oechoaround %actor% %actor.name% развернул%actor.g% скатерть.
osend %actor% Вы аккуратно развернули скатерть.
oload mob 35710
calcuid cover 35727 obj
opurge %cover%
~
#35706
свернуть скатерть~
0 c 100
свернуть~
if (%actor.clan%==сп)
mechoaround %actor% %actor.name% аккуратно сложил%actor.g% скатерть.
msend %actor% Вы аккуратно сложили скатерть.
mload obj 35727
wait 1
mpurge %self%
else
mechoaround %actor% %actor.name% попытал%actor.u% сложить скатерть, но та вырвалась из рук.
msend %actor% Вы попытались сложить скатерть, но она вырвалась из ваших рук.
end
~
$~
