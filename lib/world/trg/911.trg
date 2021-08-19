#91100
консоль~
2 c 3
/~
wait 1
%arg%
~
#91101
техномагическая винтовка~
1 c 1
выстрел~
if !%shot%
  eval shot 10
end
calcuid aroom %actor.realroom% room
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval dir %arg1%
eval victim %arg2.id%
if %arg1% == с
  set dir north
elseif %arg1% == ю
  set dir south
elseif %arg1% == з
  set dir west
elseif %arg1% == в
  set dir east
elseif %arg1% == вв
  set dir up
elseif %arg1% == вн
  set dir down
else
  %send% %actor% непонятное направление для выстрела
  halt
end
if %shot% < 1
  %send% %actor% вы попытались надавить на спусковой крючок, но он не сдвинулся с места. видать заряды закончились.
  eval shot 0
  halt
end
if %aroom.%dir%% == nil
  %send% %actor% там нету прохода, чтоб туда выстрелить
  halt
end
eval dir2 %aroom.%dir%%
eval vroom %victim.realroom%
if %dir2% != %vroom%
  %send% %actor% вы не нашли желаемую цель, увы
  halt
else
  calcuid vroom2 %vroom% room
  eval dmg %random.801% +199
  foreach i %vroom2.all%
    if %i% == %victim%
      %send% %victim% внезапно не пойми откуда в вас прилетел сгусток смертоносной магии
      %damage% %victim% %dmg%
    else
      %send% %i% внезапно не пойми откуда прилетел и врезался в %victim.dname% сгусток смертоносной магии
    done
  end
end
eval shot %shot% -1
global shot
%send% %actor% цель: %victim.name%, нанесено урона: %dmg%, осталось зарядов: %shot% из 10
~
#91102
заклинания~
1 c 3
каст~
dg_cast %arg%
~
$~
