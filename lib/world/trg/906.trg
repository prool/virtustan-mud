#90600
выдвегаем мост~
2 c 100
крутануть~
if !%arg.contains(ручку)%
  wsend %actor% &YИ что крутим?&n
  return 1
  halt
end
wait 1
wsend %actor% &YПриложив не мало усилий, вам удалось раскрутить шестерни механизма.&n
wechoaround %actor% &YКряхтя и скрипя зубами, %actor.name% раскрутил%actor.g% шестерни механизма.&n
wait 2s
wecho &YРаздался утробный скрежет и из-под деревянного настила пополз добротный мост.&n
wecho &YДвижение его прекратилось глухим стуком на противоположном берегу.&n
wecho &YТеперь стало возможным перейти глубокий ров.&n
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
  %echo% &RКакая-то тень огромного животного нависла над вами.&n
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
  %echo% &RКакая-то тень огромного животного нависла над вами.&n
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
  %echo% &RКакая-то тень огромного животного нависла над вами.&n
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
  %echo% &RКакая-то тень огромного животного нависла над вами.&n
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
%send% %actor.name% &WТень вспыхнула ярким светом, ослепив ваши глаза!&n
%echoaround% %actor% &WТень вспыхнула ярким светом, ослепив глаза %actor.dname%!&n
mecho &RВы попытались ударить тень, а ее уже и след простыл!&n
mteleport %self% 90699
~
#90607
померла белая волчица~
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
  wecho &WЗасверкали вокруг молнии, грянул ужасный гром и появился&n &RВолчий дух!&n
  wait 1s
  %force% %duxvolka% взя все.тру
  wait 1s
  wecho &RВолчий дух&n &Yрастворился в пустоте со своей ношей!&n
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
  wecho &WЗасверкали вокруг молнии, грянул ужасный гром и появился&n &RВолчий дух!&n
  wait 1s
  %force% %duxvolka% взя все.тру
  wait 1s
  wecho &RВолчий дух&n &Yрастворился в пустоте со своей ношей!&n
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
  wecho &WЗасверкали вокруг молнии, грянул ужасный гром и появился&n &RВолчий дух!&n
  wait 1s
  %force% %duxvolka% взя все.тру
  wait 1s
  wecho &RВолчий дух&n &Yрастворился в пустоте со своей ношей!&n
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
  wecho &WЗасверкали вокруг молнии, грянул ужасный гром и появился&n &RВолчий дух!&n
  wait 1s
  %force% %duxvolka% взя все.тру
  wait 1s
  wecho &RВолчий дух&n &Yрастворился в пустоте со своей ношей!&n
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
  wecho &WЗасверкали вокруг молнии, грянул ужасный гром и появился&n &RВолчий дух!&n
  wait 1s
  %force% %duxvolka% взя все.тру
  wait 1s
  wecho &RВолчий дух&n &Yрастворился в пустоте со своей ношей!&n
  %teleport% %duxvolka% 90605
end
~
#90618
разбудили мышей в клетку~
2 c 1
г го гов гово говор говори говорит говорить~
%echoaround% &W%actor% %actor.iname% сказал%actor.g% : '%arg%'&n
%send% %actor% Вы сказали : '&W%arg%&n'
wecho &W%arg%!&n
wecho &W%arg%! %arg%!&n
wecho &W%arg%! %arg%! %arg%!&n
%send% %actor.name% &yСлова из ваших уст разлетелись раскатистым эхом!&n
%echoaround% %actor% &yСлова из уст&n &R%actor.rname%&n &yразлетелись раскатистым эхом!&n
wait 2s
wecho &RНад вашей головой что-то закопошилось и помещение наполнил тонкий, пронзительный писк!&n
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
%echoaround% %actor% %actor.iname% сообщил%actor.g% группе : '&W%arg%&W'
%send% %actor% Вы сообщили группе : '&W%arg%&n'
wecho &W%arg%!&n
wecho &W%arg%! %arg%!&n
wecho &W%arg%! %arg%! %arg%!&n
%send% %actor.name% &yСлова из ваших уст разлетелись раскатистым эхом!&n
%echoaround% %actor% &yСлова из уст&n &R%actor.rname%&n &yразлетелись раскатистым эхом!&n
wait 2s
wecho &RНад вашей головой что-то закопошилось и помещение наполнил тонкий, пронзительный писк!&n
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
wecho &W%arg%!&n
wecho &W%arg%! %arg%!&n
wecho &W%arg%! %arg%! %arg%!&n
%send% %actor.name% &yСлова из ваших уст разлетелись раскатистым эхом!&n
%echoaround% %actor% &yСлова из уст&n &R%actor.rname%&n &yразлетелись раскатистым эхом!&n
wait 2s
wecho &RНад вашей головой что-то закопошилось и помещение наполнил тонкий, пронзительный писк!&n
wait 2s
foreach mouse %self.npc%
  %force% %mouse% просн
  %force% %mouse% встат
  %force% %mouse% колд !суд бог!
done
~
#90621
transform западного командира в черного оборотня~
0 ab 100
~
if (%weather.sunlight% == рассвет) && (%lastlight% != рассвет)
  eval lastlight рассвет
  global lastlight
  mecho &gПризрачные лучи кровавой луны скользнули в последний раз по шкуре %self.rname%.&n
  mecho &gПовсюду раскатистым эхом пронесся жалобный вой могучих животных.&n
  mecho &gЯрость в глазах %self.rname% угасла и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90619
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == закат) && (%lastlight% != закат)
  eval lastlight закат
  global lastlight
  mecho &gПризрачные лучи кровавой луны ласково прикоснулись к коже %self.rname%.&n
  mecho &gПовсюду прокатился раскатистым эхом воинствующий рык могучих животных.&n
  mecho &gГлаза %self.rname% сверкнули яростным блеском и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90620
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == ночь) && (%lastlight% != ночь)
  eval lastlight ночь
  global lastlight
  mecho &gПризрачные лучи кровавой луны ласково прикоснулись к коже %self.rname%.&n
  mecho &gПовсюду прокатился раскатистым эхом воинствующий рык могучих животных.&n
  mecho &gГлаза %self.rname% сверкнули яростным блеском и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90620
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == день) && (%lastlight% != день)
  eval lastlight день
  global lastlight
  mecho &gПризрачные лучи кровавой луны скользнули в последний раз по шкуре %self.rname%.&n
  mecho &gПовсюду раскатистым эхом пронесся жалобный вой могучих животных.&n
  mecho &gЯрость в глазах %self.rname% угасла и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90619
  mecho &RПеред вами внезапно возник %self.name%.&n
end
~
#90622
смерть моба с алой кровью~
0 f 100
~
mload obj 90601
eval tmp %world.obj(90601)%
%tmp.put(%self.room%)%
%echo% &YБездыханное тело %self.rname% рухнуло к вашим ногам!&n
%echo% &YМногочисленные струи&n &RАЛОЙ КРОВИ&n &Yхлынули из поверженного вами врага.&n
~
#90623
молодой вампир сумонит летучих мышей~
0 k 100
~
%echo% &R%self.name% оцарапал%self.g% левую кисть и окрапил%self.g% алой кровью потолок над головой.&n
calcuidall list 90622 mob
foreach blekmouce %list%
  %echo% &R%blekmouce.iname% явилась на зов крови %self.rname%.&n
  %teleport% %blekmouce% %self.name%
  %force% %blekmouce% атак %actor.name%
done
detach 90623 %self.id%
~
#90624
смерть моба с бурой кровью~
0 f 100
~
mload obj 90604
eval tmp %world.obj(90604)%
%tmp.put(%self.room%)%
%echo% &YБездыханное тело %self.rname% рухнуло к вашим ногам!&n
%echo% &YМногочисленные струи&n &RБУРОЙ КРОВИ&n &Yхлынули из поверженного вами врага.&n
~
#90625
набераем алую кровь~
1 g 100
~
if !%actor.haveobj(90602)%
  %echoaround% %actor% &R%actor.iname%&n &Yприсел%actor.g% на корточки и стал%actor.g% собирать&n &Rпролитую на землю алую кровь.&n
  %send% %actor.name% &YВы присели на корточки и стали собирать&n &Rпролитую на землю алую кровь.&n
  %echoaround% %actor% &YНо только&n &Rвымазал%actor.g% руки в крови по самые локти&n &Yи не собрал%actor.g%, ни капли.&n
  %send% %actor.name% &YНо только&n &Rвымазали руки в крови по самые локти&n &Yи не собрали, ни капли.&n
  return 0
  halt
end
otransform 90603
%echoaround% %actor% &R%actor.iname%&n &Yприсел%actor.g% на корточки и стал%actor.g% собирать&n &Rпролитую на землю алую кровь.&n
%send% %actor.name% &YВы присели на корточки и стали собирать&n &Rпролитую на землю алую кровь.&n .
%echoaround% %actor% &RВымазав руки в крови по самые локти, %actor.ename%&n &Yсобрал%actor.g% все до последней капли в&n &gстеклянную колбу.&n
%send% %actor.name% &RВымазав руки в крови по самые локти,&n &Yвы собрали все до последней капли в&n &Yстеклянную колбу.&n
calcuid kolba 90602 obj
%purge% %kolba%
detach 90625 %self.id%
~
#90626
набераем бурую кровь~
1 g 100
~
if !%actor.haveobj(90602)%
  %echoaround% %actor% &R%actor.iname%&n &Yприсел%actor.g% на корточки и стал%actor.g% собирать&n &Rпролитую на землю бурую кровь.&n
  %send% %actor.name% &YВы присели на корточки и стали собирать&n &Rпролитую на землю бурую кровь.&n
  %echoaround% %actor% &YНо только&n &Rвымазал%actor.g% руки в крови по самые локти&n &Yи не собрал%actor.g%, ни капли.&n
  %send% %actor.name% &YНо только&n &Rвымазали руки в крови по самые локти&n &Yи не собрали, ни капли.&n
  return 0
  halt
end
otransform 90605
%echoaround% %actor% &R%actor.iname%&n &Yприсел%actor.g% на корточки и стал%actor.g% собирать&n &Rпролитую на землю бурую кровь.&n
%send% %actor.name% &YВы присели на корточки и стали собирать&n &Rпролитую на землю бурую кровь.&n
%echoaround% %actor% &RВымазав руки в крови по самые локти, %actor.ename%&n &Yсобрал%actor.g% все до последней капли в&n &gстеклянную колбу.&n
%send% %actor.name% &RВымазав руки в крови по самые локти,&n &Yвы собрали все до последней капли в&n &Yстеклянную колбу.&n
calcuid kolba 90602 obj
%purge% %kolba%
detach 90626 %self.id%
~
#90627
набераем голубую кровь~
1 g 100
~
if !%actor.haveobj(90602)%
  %echoaround% %actor% &R%actor.iname%&n &Yприсел%actor.g% на корточки и стал%actor.g% собирать&n &Cпролитую на землю голубую кровь.&n
  %send% %actor.name% &YВы присели на корточки и стали собирать&n &Cпролитую на землю голубую кровь.&n
  %echoaround% %actor% &YНо только&n &Cвымазал%actor.g% руки в крови по самые локти&n &Yи не собрал%actor.g%, ни капли.&n
  %send% %actor.name% &YНо только&n &Cвымазали руки в крови по самые локти&n &Yи не собрали, ни капли.&n
  return 0
  halt
end
otransform 90607
%echoaround% %actor% &R%actor.iname%&n &Yприсел%actor.g% на корточки и стал%actor.g% собирать&n &Cпролитую на землю голубую кровь.&n
%send% %actor.name% &YВы присели на корточки и стали собирать&n &Cпролитую на землю голубую кровь.&n
%echoaround% %actor% &CВымазав руки в крови по самые локти,&n &R%actor.ename%&n &Yсобрал%actor.g% все до последней капли в&n &gстеклянную колбу.&n
%send% %actor.name% &CВымазав руки в крови по самые локти,&n &Yвы собрали все до последней капли в&n &Yстеклянную колбу.&n
detach 90627 %self.id%
~
#90628
смерть моба с голубой кровью~
0 f 100
~
mload obj 90606
eval tmp %world.obj(90606)%
%tmp.put(%self.room%)%
%echo% &YБездыханное тело %self.rname% рухнуло к вашим ногам!&n
%echo% &YМногочисленные струи&n &CГОЛУБОЙ КРОВИ&n &Yхлынули из поверженного вами врага.&n
~
#90629
transform восточного командира в бурого оборотня~
0 ab 100
~
if (%weather.sunlight% == рассвет) && (%lastlight% != рассвет)
  eval lastlight рассвет
  global lastlight
  mecho &gПризрачные лучи кровавой луны скользнули в последний раз по шкуре %self.rname%.&n
  mecho &gПовсюду раскатистым эхом пронесся жалобный вой могучих животных.&n
  mecho &gЯрость в глазах %self.rname% угасла и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90645
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == закат) && (%lastlight% != закат)
  eval lastlight закат
  global lastlight
  mecho &gПризрачные лучи кровавой луны ласково прикоснулись к коже %self.rname%.&n
  mecho &gПовсюду прокатился раскатистым эхом воинствующий рык могучих животных.&n
  mecho &gГлаза %self.rname% сверкнули яростным блеском и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90646
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == ночь) && (%lastlight% != ночь)
  eval lastlight ночь
  global lastlight
  mecho &gПризрачные лучи кровавой луны ласково прикоснулись к коже %self.rname%.&n
  mecho &gПовсюду прокатился раскатистым эхом воинствующий рык могучих животных.&n
  mecho &gГлаза %self.rname% сверкнули яростным блеском и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90646
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == день) && (%lastlight% != день)
  eval lastlight день
  global lastlight
  mecho &gПризрачные лучи кровавой луны скользнули в последний раз по шкуре %self.rname%.&n
  mecho &gПовсюду раскатистым эхом пронесся жалобный вой могучих животных.&n
  mecho &gЯрость в глазах %self.rname% угасла и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90645
  mecho &RПеред вами внезапно возник %self.name%.&n
end
~
#90630
transform свирепого воина и свирепого оборотня~
0 ab 100
~
if (%weather.sunlight% == рассвет) && (%lastlight% != рассвет)
  eval lastlight рассвет
  global lastlight
  mecho &gПризрачные лучи кровавой луны скользнули в последний раз по шкуре %self.rname%.&n
  mecho &gПовсюду раскатистым эхом пронесся жалобный вой могучих животных.&n
  mecho &gЯрость в глазах %self.rname% угасла и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90647
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == закат) && (%lastlight% != закат)
  eval lastlight закат
  global lastlight
  mecho &gПризрачные лучи кровавой луны ласково прикоснулись к коже %self.rname%.&n
  mecho &gПовсюду прокатился раскатистым эхом воинствующий рык могучих животных.&n
  mecho &gГлаза %self.rname% сверкнули яростным блеском и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90648
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == ночь) && (%lastlight% != ночь)
  eval lastlight ночь
  global lastlight
  mecho &gПризрачные лучи кровавой луны ласково прикоснулись к коже %self.rname%.&n
  mecho &gПовсюду прокатился раскатистым эхом воинствующий рык могучих животных.&n
  mecho &gГлаза %self.rname% сверкнули яростным блеском и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90648
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == день) && (%lastlight% != день)
  eval lastlight день
  global lastlight
  mecho &gПризрачные лучи кровавой луны скользнули в последний раз по шкуре %self.rname%.&n
  mecho &gПовсюду раскатистым эхом пронесся жалобный вой могучих животных.&n
  mecho &gЯрость в глазах %self.rname% угасла и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90647
  mecho &RПеред вами внезапно возник %self.name%.&n
end
~
#90631
transform жестокого воина и жестокого оборотня~
0 ab 100
~
if (%weather.sunlight% == рассвет) && (%lastlight% != рассвет)
  eval lastlight рассвет
  global lastlight
  mecho &gПризрачные лучи кровавой луны скользнули в последний раз по шкуре %self.rname%.&n
  mecho &gПовсюду раскатистым эхом пронесся жалобный вой могучих животных.&n
  mecho &gЯрость в глазах %self.rname% угасла и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90649
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == закат) && (%lastlight% != закат)
  eval lastlight закат
  global lastlight
  mecho &gПризрачные лучи кровавой луны ласково прикоснулись к коже %self.rname%.&n
  mecho &gПовсюду прокатился раскатистым эхом воинствующий рык могучих животных.&n
  mecho &gГлаза %self.rname% сверкнули яростным блеском и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90650
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == ночь) && (%lastlight% != ночь)
  eval lastlight ночь
  global lastlight
  mecho &gПризрачные лучи кровавой луны ласково прикоснулись к коже %self.rname%.&n
  mecho &gПовсюду прокатился раскатистым эхом воинствующий рык могучих животных.&n
  mecho &gГлаза %self.rname% сверкнули яростным блеском и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90650
  mecho &RПеред вами внезапно возник %self.name%.&n
end
if (%weather.sunlight% == день) && (%lastlight% != день)
  eval lastlight день
  global lastlight
  mecho &gПризрачные лучи кровавой луны скользнули в последний раз по шкуре %self.rname%.&n
  mecho &gПовсюду раскатистым эхом пронесся жалобный вой могучих животных.&n
  mecho &gЯрость в глазах %self.rname% угасла и он не ожиданно перекувыркнулся через голову.&n
  mtransform 90649
  mecho &RПеред вами внезапно возник %self.name%.&n
end
~
#90632
девушка сумонит змей~
0 k 100
~
%echo% &R%self.name% оцарапал%self.g% левую кисть и окрапил%self.g% алой кровью все вокруг себя.&n
calcuidall list 90626 mob
foreach zmea %list%
  %echo% &R%zmea.iname% приползла на зов крови %self.rname%.&n
  %teleport% %zmea% %self.name%
  %force% %zmea% атак %actor.name%
done
detach 90632 %self.id%
~
#90633
девушка сумонит пантер~
0 k 100
~
%echo% &R%self.name% укусил%self.g% левую кисть и окрапил%self.g% алой кровью пол у ваших ног.&n
calcuidall list 90628 mob
foreach pantera %list%
  %echo% &R%pantera.iname% прибежала на зов крови %self.rname%.&n
  %teleport% %pantera% %self.name%
  %force% %pantera% атак %actor.name%
done
detach 90633 %self.id%
~
#90634
паук холдит~
0 k 100
~
if %random.100% < 30
  dg_affect %actor.name% обездвижен оцепенение 2 2
  msend %actor.name% _&R%self.name%&n &Wмолниеносным движением накинул на вас сеть из паутины, и вы замерли на месте запутавшись вней&n
  mechoaround %actor% _&R%self.name%&n &Wмолниеносным движением накинул сеть из паутины на&n &R%actor.vname%&n &Wи он%actor.g% замер%actor.q% на месте, запутавшись вней!&n
end
~
#90635
мгла холдит и телепортит~
0 b 50
~
wait 1
set victim %random.pc%
if !%victim%
  halt
end
if (%random.100% <= 70)
  msend %victim% &WСвинцовая мгла разверглась на ваших глазах,&n &Cи вы угодили прямо в ее ледяные объятья!&n
  msend %victim% &CЛедяные оковы ограничили ваше движенье&n, &Wи вы рухнули покрытые инеем  прямо под мост в вонючую жижу.&n
  mechoaround %victim% &WСвинцовая мгла разверглась на ваших глазах,&n &Cи %victim.name% угодил%victim.g% прямо в ее ледяные объятья!&n 
  mechoaround %victim% &CЛедяные оковы сковали %victim.vname%&n, &Wи он%victim.g% рухнул%victim.g% покрыт%victim.w% инеем  прямо под мост в вонючую жижу.&n
  dg_affect %victim% обездвижен оцепенение 2 30
  mteleport %victim% 90691
  mechoaround %victim% &CЛедяная статуя, чем-то напоминающая %victim.vname%,&n &Wрухнула к вашим ногам, подняв кучу брызг вонючей жижи.&n
end
~
#90636
запуск хила погонщика~
0 k 100
~
wait 5s
calcuid room90607 90607 room
attach 90637 %room90607.id%
exec 90637 %room90607.id%
~
#90637
хил погонщика~
2 z 100
~
wait 1
if %exist.mob(90610)% && %exist.mob(90651)%
  calcuid szla 90651 mob
  wait 4s
  %teleport% %szla% 90607
  wecho &WВ небе сгустились свинцовые тучи, и над вами нависли ужасающие&n &RСИЛЫ ЗЛА!&n
  wait 1s
  %force% %szla% атак .%actor.name%
end
detach 90637 %self.id%
~
#90638
силы зла хиляют~
0 v 1
~
wait 1
if (%actor.vnum% == -1)
  dg_cast 'выс жизн' .%actor.name%
else
  dg_cast 'выс жизн' .%actor.name%
end
%send% %actor.name% &RСИЛЫ ЗЛА&n &rокутали вас с головы до ног, и стали высасывать все ваши жизненые соки!&n
%echoaround% %actor% &RСИЛЫ ЗЛА&n &rокутали&n &R%actor.dname%&n &rс головы до ног, и стали высасывать из жертвы все жизненные соки!&n
wait 1s
dg_cast 'исц' погонщ
mecho &rОдарив жизненной силой&n &ПОГОНЩИКА ТЕМНЫХ ПСОВ, RСИЛЫ ЗЛА&n &rрассеялись словно туман.&n
mteleport %self% 90699
attach 90636 %self.id%
~
$~
