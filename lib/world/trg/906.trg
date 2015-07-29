#90600
выдвегаем мост~
2 c 100
крутануть~
if !%arg.contains(ручку)%
  wsend %actor% И что крутим?
  return 1
  halt
end
wait 1
wsend %actor% Приложив не мало усилий, вам удалось раскрутить шестерни механизма.
wechoaround %actor% Кряхтя и скрипя зубами, %actor.name% раскрутил%actor.g% шестерни механизма.
wait 2s
wecho Раздался утробный скрежет и из-под деревянного настила пополз добротный мост.
wecho Движение его прекратилось глухим стуком на противоположном берегу.
wecho Теперь стало возможным перейти глубокий ров.
wdoor 90605 north flags a
wdoor 90605 north room  90606wdoor 90606 south   flags a
wdoor 90606 south   room  90605
detach 90600 %self.id%
~
#90601
репоп~
2 f 100
~
wait 1
wdoor 90605 north purge
wdoor 90606 south   purge
calcuid qroom 90605 room
attach 90600 %qroom.id%
~
#90602
появление тени~
2 e 100
~
wait 1s
if %exist.mob(90601)%
  calcuid tenvolka 90601 mob
  %teleport% %tenvolka% 90601
  %echo% Какая-то тень огромного животного нависла над вами.
  wait 1s
  %force% %tenvolka% атак .%actor.name%
end
~
#90603
появление тени 2~
2 e 100
~
wait 1s
if %exist.mob(90601)%
  calcuid tenvolka 90601 mob
  %teleport% %tenvolka% 90602
  %echo% Какая-то тень огромного животного нависла над вами
  %force% %tenvolka% атак .%actor.name%
end
~
#90604
появление тени 3~
2 e 100
~
wait 1s
if %exist.mob(90601)%
  calcuid tenvolka 90601 mob
  %teleport% %tenvolka% 90603
  %echo% Какая-то тень огромного животного нависла над вами
  %force% %tenvolka% атак .%actor.name%
end
~
#90605
появление тени 4~
2 e 100
~
wait 1s
if %exist.mob(90601)%
  calcuid tenvolka 90601 mob
  %teleport% %tenvolka% 90604
  %echo% Какая-то тень огромного животного нависла над вами
  %force% %tenvolka% атак .%actor.name%
end
~
#90606
тень ослепляет~
0 v 1
~
wait 1
if (%actor.vnum% == -1)
  dg_cast 'слепот' .%actor.name%
else
  dg_cast 'слепот' %actor.name%
end
%send% %actor.name% Тень вспыхнула ярким светом, ослепив ваши глаза!
%echoaround% %actor% Тень вспыхнула ярким светом, ослепив глаза %actor.dname%!
mecho Вы попытались ударить тень, а ее уже и след простыл.
mteleport %self% 90699
~
#90607
померла волчица~
0 f 100
~
calcuid room90600 90600 room
attach 90608 %room90600.id%
exec 90608 %room90600.id%
detach 90607 %self.id%
~
#90608
дух ворует труп белой волчицы~
2 z 100
~
wait 1
if %exist.mob(90602)%
  calcuid duxvolka 90602 mob
  %teleport% %duxvolka% 90600
  wecho Засверкали вокруг молнии, грянул ужасный гром и появился волчий дух!
  wait 1s
  %force% %duxvolka% взя все.тру
  wait 1s
  wecho Волчий дух растворился в пустоте со своей ношей!
  %teleport% %duxvolka% 90605
end
~
#90609
поднимает волчий дух трупов~
0 q 100
~
wait 1
брос все.тру
dg_cast 'ожив труп'  труп
dg_cast 'ожив труп'  труп
dg_cast 'ожив труп'  труп
dg_cast 'поднять труп'  труп
dg_cast 'поднять труп'  труп
dg_cast 'поднять труп'  труп
взя все
приказ всем убить .%actor.name%
~
#90610
померла серая волчица~
0 f 100
~
calcuid room90601 90601 room
attach 90611 %room90601.id%
exec 90611 %room90601.id%
detach 90610 %self.id%
~
#90611
волчий дух ворует труп серой волчицы~
2 z 100
~
wait 1
if %exist.mob(90602)%
  calcuid duxvolka 90602 mob
  %teleport% %duxvolka% 90601
  wecho Засверкали вокруг молнии, грянул ужасный гром и появился волчий дух!
  wait 1s
  %force% %duxvolka% взя все.тру
  wait 1s
  wecho Волчий дух растворился в пустоте со своей ношей!
  %teleport% %duxvolka% 90605
end
~
#90612
помер белый волк~
0 f 100
~
calcuid room90602 90602 room
attach 90613 %room90602.id%
exec 90613 %room90602.id%
detach 90612 %self.id%
~
#90613
дух ворует труп белого волка~
2 z 100
~
wait 1
if %exist.mob(90602)%
  calcuid duxvolka 90602 mob
  %teleport% %duxvolka% 90602
  wecho Засверкали вокруг молнии, грянул ужасный гром и появился волчий дух!
  wait 1s
  %force% %duxvolka% взя все.тру
  wait 1s
  wecho Волчий дух растворился в пустоте со своей ношей!
  %teleport% %duxvolka% 90605
end
~
#90614
помер серый волк~
0 f 100
~
calcuid room90603 90603 room
attach 90615 %room90603.id%
exec 90615 %room90603.id%
detach 90614 %self.id%
~
#90615
дух ворует труп серого волка~
2 z 100
~
wait 1
if %exist.mob(90602)%
  calcuid duxvolka 90602 mob
  %teleport% %duxvolka% 90603
  wecho Засверкали вокруг молнии, грянул ужасный гром и появился волчий дух!
  wait 1s
  %force% %duxvolka% взя все.тру
  wait 1s
  wecho Волчий дух растворился в пустоте со своей ношей!
  %teleport% %duxvolka% 90605
end
~
#90616
помер вожак волков~
0 f 100
~
calcuid room90604 90604 room
attach 90617 %room90604.id%
exec 90617 %room90604.id%
detach 90616 %self.id%
~
#90617
дух ворует труп вожака волков~
2 z 100
~
wait 1
if %exist.mob(90602)%
  calcuid duxvolka 90602 mob
  %teleport% %duxvolka% 90604
  wecho Засверкали вокруг молнии, грянул ужасный гром и появился волчий дух!
  wait 1s
  %force% %duxvolka% взя все.тру
  wait 1s
  wecho Волчий дух растворился в пустоте со своей ношей!
  %teleport% %duxvolka% 90605
end
~
#90618
разбудили мышей в клетку~
2 c 1
г го гов гово говор говори говорит говорить~
%echoaround% %actor% %actor.iname% сказал%actor.g% : '%arg%'
%send% %actor% Вы сказали : '%arg%'
wecho %arg%!
wecho %arg%! %arg%!
wecho %arg%! %arg%! %arg%!
%send% %actor.name% Слова из ваших уст разлетелись раскатистым эхом!
%echoaround% %actor% Слова из уст %actor.rname% разлетелись раскатистым эхом!
wait 2s
wecho Над вашей головой что-то закопошилось и помещение наполнил тонкий, пронзительный писк!
wait 2s
foreach mouse %self.npc%
  %force% %mouse% просн
  %force% %mouse% встат
  %force% %mouse% колд !суд бог!
done
~
#90619
разбудиили мышек в группу~
2 c 1
гг ггр ггру ггруп ггрупп ггруппа~
%echoaround% %actor% %actor.iname% сообщил%actor.g% группе : '%arg%'
%send% %actor% Вы сообщили группе : '%arg%'
wecho %arg%!
wecho %arg%! %arg%!
wecho %arg%! %arg%! %arg%!
%send% %actor.name% Слова из ваших уст разлетелись раскатистым эхом!
%echoaround% %actor% Слова из уст %actor.rname% разлетелись раскатистым эхом!
wait 2s
wecho Над вашей головой что-то закопошилось и помещение наполнил тонкий, пронзительный писк!
wait 2s
foreach mouse %self.npc%
  %force% %mouse% просн
  %force% %mouse% встат
  %force% %mouse% колд !суд бог!
done
~
#90620
разбудили мышек в гд~
2 c 1
гд гдр гдру гдруг гдруга гдругам~
%echoaround% %actor% %actor.iname% дружине : &R'%arg%'.&n
%send% %actor% Вы дружине : &R'%arg%'.&n
wecho %arg%!
wecho %arg%! %arg%!
wecho %arg%! %arg%! %arg%!
%send% %actor.name% Слова из ваших уст разлетелись раскатистым эхом!
%echoaround% %actor% Слова из уст %actor.rname% разлетелись раскатистым эхом!
wait 2s
wecho Над вашей головой что-то закопошилось и помещение наполнил тонкий, пронзительный писк!
wait 2s
foreach mouse %self.npc%
  %force% %mouse% просн
  %force% %mouse% встат
  %force% %mouse% колд !суд бог!
done
~
#90621
превращение в человека~
0 b 100
~
wait 1
if (%weather.sunlight% == рассвет)
  крич наступил день
end
~
#90622
превращение в волка~
0 ab 100
~
wait 1
if (%weather.sunlight% == закат)
  крич наступила ночь
end
~
$~
