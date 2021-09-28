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
eval rroom %actor.realroom%
eval arg1 %arg.car%
eval arg2 %arg.words(2)%
switch %arg1.mudcommand%
  case север
    eval dir %rroom.north%
    set tdir на север
    set tdir2 с юга
  break
  case юг
    eval dir %rroom.south%
    set tdir на юг
    set tdir2 с севера
  break
  case запад
    eval dir %rroom.west%
    set tdir на запад
    set tdir2 с востока
  break
  case восток
    eval dir %rroom.east%
    set tdir на восток
    set tdir2 с запада
  break
  case вверх
    eval dir %rroom.up%
    set tdir вверх
    set tdir2 снизу
  break
  case вниз
    eval dir %rroom.down%
    set tdir вниз
    set tdir2 сверху
  break
  default
    %send% %actor% непонятное направление
    halt
  break
done
if !%dir%
  %send% %actor% в указанном направлении нет комнаты
  halt
end
foreach i %dir.all%
  if %i.iname% /= %arg2%
    eval victim %i%
    eval vexp %victim.exp%
  break
end
done
if !%victim%
  %send% %actor% вы не видите цель
  halt
end
%send% %actor% вы выстрелили %tdir% в %victim.vname%
%echoaround% %actor% %actor.iname% выстрелил%actor.g% %tdir%
%send% %victim% %actor.iname% выстрелил%actor.g% %tdir2%
oat %dir% %echoaround% %victim% кто-то выстрелил в %victim.vname% %tdir2%
%damage% %victim% 1000
%actor.wait(2)%
if %victim.position% == 0
  eval exp1 %actor.exp%
  %actor.exp(+%vexp%)%
  eval exp2 %actor.exp%-%exp1%
  if %exp2% == 0
    %send% %actor% вы не получили опыта
  else
    %send% %actor% получено опыта: %exp2% единиц
  end
end
~
#91102
заклинания~
1 c 3
каст~
attach 91103 %actor%
exec 91103 %actor%
detach 91103 %actor%
~
#91103
кастуем закл~
0 z 100
~
dg_cast %arg.car% '%arg.cdr%'
~
$~
