#8000
заход чара к стражам~
0 q 100
~
emot внимательно осмотрел Вас.
wait 3
say Уходи отсюда путник, пока жив
if !%questor47%
  say Хотя... Возможно ты поможешь в одном деле?
  attach 8001 %self.id%
else
  say Уходи...
end
~
#8001
берем задание~
0 d 100
помогу да~
say ОК
if %questor47.id% ==  %actor.id%
  say Ты уже согласился помочь!
  halt
end
if %questor47%
  wait 2
  say Извини, но %questor47.vname% уже выполняет мое задание...
  halt
end
if (%world.curmobs(8016)% == 0)
  halt
end
set questor47 %actor%
calcuid prizrak 8016 mob
attach 8015 %prizrak.id%
global questor47
remote questor47 %prizrak.id%
say ок
.ы
~
#8002
убили призрака~
0 f 0
~
if %actor.vnum% == -1
  set killer47 %actor%
else
  set killer47 %actor.leader%
end
global killer47
calcuid hunter 8000 mob
remote killer47 %hunter.id%
mload obj 8002
~
#8003
награда~
0 j 0
~
wait 1
if %object.vnum% != 8002
  say Зачем  мне?
  drop all
  halt
end
wait 1
mjunk all
if %actor.id% != %questor47.id%
  say Я не просил Вас о помощи...
  поклон .%actor.name%
  detach 8001 %self.id%
  halt
end
if %actor.id% != %killer47.id%
  say Не ты убил его!
  гнев
  detach 8003 %self.id%
  halt
end
say Молодец!
wait 1
say Держи немного кун в знак благодарности
%self.gold(5000)%
give 5000 кун .%actor.name%
~
$~
