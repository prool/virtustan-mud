* BRusMUD trigger file v1.0
#37700
на водопаде~
2 e0 100
~
if %actor.vnum% != -1
  halt
end
wait 5s
wecho _Вдруг внезапно поток воды обрущился на Вас, и смыл Вас с камней!
wait 5
wecho _Вы полетели в водную пропасть!
wteleport all 37754
end










~
#37701
зашел к хранителю вечности~
0 r0 100
~
if %actor.level% > 31
wait 5
say Здраве тебе о Великий Бог %actor.name%!
say Я всю жизнь ждал твоего появления!
wait 5
поклон %actor.name%
wiat 5
say Я не могу сейчас идти с тобой, у меня миссия здесь, я буду ее выполнять!
else
wait 5
set target %random.pc%
eval rrr %random.100%
if %rrr% < 4
          eval  exp %target.exp%
          eval  exp %exp%/30
          %target.exp(-%exp%)%
          msend %actor% _Размашистый удар хранителя вечности попал Вам в голову!
          msend %actor% _Вы потеряли %exp% очков опыта. 
          mkill %actor.name% 
else
           eval hp %target.hitp%
           eval hp %hp%/2
           %target.hitp(-%hp%)%
           mkill %actor.name%
           msend %actor% _Размашистый удар хранителя вечности попал Вам в живот!
           msend %actor%  _Это действительно БОЛЬНО!
end
detach 37701 %self.id%


   






~
#37702
зашел к хранителю молодости~
0 r0 100
~
if %actor.level% > 31
wait 5
say Здраве тебе о Великий Бог %actor.name%!
say Я всю жизнь ждал твоего появления!
wait 5
поклон %actor.name%
wiat 5
say Я не могу сейчас идти с тобой, у меня миссия здесь, я буду ее выполнять!
else
wait 5
say Что Вы тут делаете ?
say Мне придется убить Вас!
wait 5
say Первый раз за две сотни лет я вижу людей.
wait 5
say Ладно откадайте мою загадку, и я еще подумаю!
wait 10
  while 1
     switch %random.3%
          case 1
               say В чем суть жизни?
               set slovo молодость
               global slovo
             break
          case 2
                say Сколько лет я здесь сижу?
                set slovo тысяча
                global slovo
              break
          case 3
                say Зачем я здесь сижу?
                set slovo защита
                global slovo
              break
       done
end
attach 37703 %self.id%
end





~
#37703
сказал овет~
0 d0 0
*~
if (%speech%==%slovo%)
say Да, Вы правы!
say Но все равно мне придется Вас убить!
calcuid komn 37717 room
attach 37704 %komn.id%
exec 37704 %komn.id%
detach 37702 %self.id%
detach 37703 %self.id%
else
say Нет, Вы не правы, Вы будете убиты!
mkill %actor.name%
end





~
#37704
***~
2 z0 100
~
calcuid mob 37705 mob
wpurge %mob%
wload mob 37702
calcuid mob 37702 mob
set rand %random.pc%
wforce %mob.name% атак %rand.name%
end






~
#37705
зашел к белой лошади~
0 r0 100
~
wiat 10
mecho _Белая лошадь заметила Вас!
wait 5
mecho _Из Ноздрей белой лошади повалил дым, и она кинлуась в атаку!
wiat 5
set target %random.pc%
mesend %target% _Целью она выбрала Вас!
mkill %target.name%
attach 37706 %self.id%
detach 37705 %self.id%
end

~
#37706
в бою с белой лошадью~
0 l0 100
~
if %self.hitp% < 600
wait 2
mecho _Из ноздрей белой лошади повалил дым, ослепляя Вас!
set target %random.pc%
dg_cast 'массовая слепота' %target.name%
mload mob 37707
mecho _На помощь белой лошади прибыл жеребенок!
mload mob 37707
mecho _На помощь белой лошади прибыл жеребенок!
mload mob 37707
mecho _На помощь белой лошади прибыл жеребенок!
detach 37706 %self.id%
end




~
#37707
в бою с черной лошадью~
0 k0 100
~
if %world.curmobs(37706)% < 3
mecho _На помощь черной лошади прибыл жеребенок!
mload mob 37706
end






~
#37708
открыл гроб~
1 p0 100
~
wait  1
oload  mob 37709
osend  %actor% _Внезапно труп ожил, и ринулся на Вас!
oechoaround  %actor% Внезапно труп ожил, и ринулся на %actor.vname%!
oforce зомби attack %actor.name%
opurge гроб
detach 37708 %self.id%
end



~
#37709
взяли останки~
1 g0 100
~
wait 1
oecho _Внезапно какие-то злые силы подхватили останки.
wait 1
oecho _И они начали крутиться в воздухе!
wait 5
oecho _Все быстрее и быстрее!
wait 5
oecho _Опустился густой туман!
wait 10
oecho _Туман рассеялся, и Вы увидили........
oload mob 37710
wait 4
oforce властелин атак %actor.name%
opurge останки
detach 37709 %self.id%
end

~
#37710
в бою с властелином~
0 k0 100
~
if (%world.curobjs(37701)% > 0) && (%world.curmobs(37709)% < 9)
wiat 2
mecho _Властелин прикрыл глаза и проборматал : "да помогут мне иные силы!"
mecho _На помощь властелину прибыл разъяренный зомби!
mload mob 37709
end



~
#37711
подвинуть камень~
1 c0 100
двигать~
if !%arg.contains(камень)%
   return 0
   halt
end
wait 5
osend %actor% _Вы подвинули камень.
oechoaround %actor% _%actor.name% подвинул%actor.q% камень.
wait 5
oecho _За камнем вы увидили какую-то яму!
odoor 37702 down flags a
odoor 37702 down room 37755
odoor 37755 up flags a
odoor 37755 up room 37702
detach 37711 %self.id%
end




~
#37712
репоп зоны~
2 f0 100
~
wdoor 37702 down purge
wdoor 37755 up purge




~
#37713
умер властелин~
0 f0 100
~
mecho _До Вас донеслись следующие звуки: "Вы убили лишь плоть, я буду существовать вокруг Вас!"
calcuid quest 37756 room
attach 37714 %quest.id%
mdoor 37753 east flags a
mdoor 37753 east room 37756
mdoor 37756 west flags a
mdoor 37756 west room 37753
end

~
#37714
поставить камнь~
2 c0 100
поставить~
if !%arg.contains(камень)%
   return 0
   halt
end
if %actor.haveobj(37703)%
  wecho  _Скрипучий звук провазглосил : "О нет! Это тот камень, который я прятал так долго! Он меня и погубил!"
  wecho _Сильный ветер закружил Вас! И когда он утих, Вы увидили что-то.....
wpurge камень
calcuid quest1 37756 room
attach 37718 %quest1.id%
exec 37718 %quest1.id%
end


~
#37715
умер хранитель вечн~
0 0 0
~
if (%world.curobjs(37705)% < 3) && (%random.10% <= 2)
   mload obj 37705
end



~
#37716
умер хран молод~
0 f0 100
~
if (%world.curobjs(37704)% < 3) && (%random.5% == 1)
   mload obj 37704
end

~
#37717
лоад буки~
2 h0z0 100
~
if (%world.curobj(37707)%==0) && (%random.50%==1)
wload obj 37707
else
wload obj 37706
end

~
#37718
quest~
2 z0 100
~
if (%world.curobj(37707)%==0) && (%random.2%==1)
wload obj 37707
end
wload obj 37706
end


~
$
$
