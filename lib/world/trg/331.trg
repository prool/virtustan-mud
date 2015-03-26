#33100
у печальной русалки~
0 q 100
~
wait 1
say Полынь или петрушка?
wait 2s
calcuid russal 33119 mob
attach 33113 %russal.id%
attach 33114 %russal.id%
~
#33101
у корол краба~
0 q 100
~
wait 1
улыб
say Здорово путник!
wait 2s
say Ты выглядишь достаточно сильным, что бы помочь мне....
wait 2s
say Местный спрут-разбойник, что живет неподалеку, неоднократно ел моих сородичей..
wait 1s 
say Мне это надоело! 
wait 1s
say Помоги убить его, и я тебя отблагодарю...
~
#33102
дал утробу крабу~
0 j 100
~
wait 1
if %object.vnum% != 33105 
  drop %object.name%
  halt
end
wait 1
mpurge %object%
wait 2s
say Не может быть! Ты смог сделать это! Вот молодец!
wait 2s
if (%world.curobjs(33110)% < 5) && (%random.4% == 1)
  mecho Краб ловко разрезал клешней утробу и достал от туда водянистый лук..
  mload obj 33110
  wait 2s
  дат лук %actor.name%
end
if (%world.curobjs(206)% < 50) && (%random.3% == 3)
  say Один знакомый утопленник оставил мне эту вещь. Может тебе пригодится?  
  mecho Краб достал из под панциря руну света.
  mload obj 206
  wait 2s
  дат рун %actor.name%
end
~
#33103
у утопленника~
0 q 100
~
wait 1
say А-а, еще один искатель Подводного царства....
wait 1s
say Я тоже когда-то искал его....
вздох
wait 2s
say Нет, я думаю, что умный в гору не пойдет. Умный гору обойдет!
wait 1s
mecho _Утопленник вздохнул, прослезился, и стал дальше искать вход в Подводное царство
~
#33104
в бою с акулой~
0 q 100
~
wait 1
mecho _Акула оскалила зубы, перевернулась на спину и, разинув пасть, кинулась в бой 
mkill %actor.name%
~
#33105
в бою со спрутом~
0 q 100
~
wait 1
mecho _Спрут нехотя отбросил в сторону останки русалки и начал упражняться на Вас
mkill %actor.name%
~
#33106
рядом с рыбкой серебристой~
0 r 100
~
wait 4s
mecho _Чешуя серебристой рыбки замерцала при свете всеми цветами радуги
~
#33107
помер спрут~
0 f 100
~
if %world.curobjs(33105)% < 1
  mload obj 33105
end
if (%world.curobjs(33101)% < 2) && (%random.4% == 1)
  mload obj 33101
end
if (%world.curobjs(502)%==0) && (%random.4%==1)
  mload obj 502
end
~
#33108
помер  к.краб~
0 f 100
0~
if (%world.curobjs(33100)% < 10) && (%random.2% == 1)
  mload obj 33100
end
~
#33109
помер  к.окраннник~
0 f 100
0~
if (%world.curobjs(33109)% < 10) && (%random.4% == 1)
  mload obj 33109
end
~
#33110
помер  огром.краб~
0 f 100
0~
if (%world.curobjs(33103)% < 8) && (%random.4% == 1)
  mload obj 33103
end
~
#33111
помер самка.краб~
0 f 100
0~
if (%world.curobjs(33104)% < 8) && (%random.5% == 1)
  wload obj 33104
end
~
#33112
помер акула~
0 f 100
0~
if (%world.curobjs(33102)% < 10) && (%random.2% == 1)
  mload obj 33102
end
~
#33113
полынь~
0 d 1
полынь~
if (%actor.vnum% != -1)
  halt
end
wait 1
say Сам ты сгинь!!!
wait 1s
msend       %actor% _Русалка размахнулась на Вас, потом передумала и отвернулась.
mechoaround %actor% _Русалка размахнулась на  %actor.vname%, потом передумала и отвернулась.  
calcuid rusall 33119 mob
detach 33100 %rusall.id%
detach 33113 %rusall.id%
detach 33114 %rusall.id%
wait 1s
mecho _Русалка быстро уплыла на юг.
mpurge %rusall.id%
end
~
#33114
петрушка~
0 d 1
петрушка~
if (%actor.vnum% != -1)
  halt
end
wait 1
say Ах ты моя душка!!!
wait 1
mecho  _Русалка улыбнулась Вам, указала на север и предупредила об опасности..
calcuid rusalk 33119 mob
detach 33100 %rusalk.id%
detach 33113 %rusalk.id%
detach 33114 %rusalk.id%
wait 1s
mecho _Русалка уплыла на север.
mecho  Вы услышали предсмертный вскрик!
mpurge %rusalk.id%
end
~
#33115
начало квеста в 33100~
2 f 100
~
calcuid rusal 33119 mob
attach 33100 %rusal.id%
~
#33116
пришел в водоворот~
2 e 100
~
wait 1s
wsend       %actor% Мощным водоворотом вас утащило под воду и вы потеряли сознание.
wechoaround %actor% %actor.name% с криком исчез%actor.q% в Водовороте!!!
wait 1s
wsend %actor.name% .- Вы очнулись на дне моря.
wteleport %actor% 33102
wat 33102 wechoaround %actor% Кто-то был выброшен сюда течением!
~
#33117
помер  рыбка~
0 f 100
0~
mload obj 33111
~
#33118
помер  хамелион~
0 f 100
0~
if (%world.curobjs(33112)% < 10) && (%random.10% <= 2)
  mload obj 33112
end
~
#33119
лава под водой~
2 e 100
~
eval temp %actor%
wait 1s
if (%actor.realroom% != %self.vnum%)
  halt
end
wsend       %temp.name% _Неожиданно на Вас обрушился поток раскаленной лавы.
wechoaround %temp.name% _Неожиданно на %actor.vname% обрушилась раскаленная лава.
wechoaround %temp.name% _Нечего гулять у Вулкана!
wdamage %temp.name% 75
~
#33120
дали что то русалке~
0 j 100
~
wait 1
calcuid predmet %object.vnum% obj
wait 1
mpurge %predmet%
wait 2
say Ну зачем мне это???  Хотя, оставим про запас.
~
$~
