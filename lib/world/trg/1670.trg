#167000
частый урон~
2 b 100
~
%echo% хаос опустошает вас
eval gopa %self.all%
foreach i %gopa%
  %damage% %i% 169
done

~
#167001
бессмертие~
0 f 100
~
%echo% раздаётся ужасающий, пробирающий до костей смех.
if %actor.sex% == 1
  %send% %actor% да ты видать позабыл, что Дариан то- бессмертный!
elseif %actor.sex% == 2
  %send% %actor% да ты видать позабыла, что дариан то бессмертный
elseif %actor.sex% == 0
  %send% %actor% да ты видать позабыло, что дариан то бессмертный
elseif %actor.sex% == 3
  %send% %actor% да вы видать позабыли, что дариан то бессмертный
end
%echo% Дариан восстаёт из пепла, его лицо перекошено в приступе дикой злобы
%load% mob 167001
гов ты забываешься, жалкий плебей!
dg_cast 'масс оц' %actor%
dg_cast 'буря от' %actor%
%force% %actor% гов извините меня, покорного слугу, я всё осознал и готов на любое наказание
%send% %actor% пошёл к чёрту, холоп!
%send% %actor% внезапно вас куда-то неудержимо затягивает.
%echoaround% %actor% %actor.name% внезапно исчезает
%teleport% %actor% 167000
~
#167002
божественная молния~
1 c 1
молния~
eval arg1 %arg.car%
eval victim %arg1%
eval dmg %arg.words(2)%
%send% %actor% вы ударили в %victim.vname% молнией на %dmg% хитов
calcuid rroom %victim.realroom% room
attach 167014 %rroom%
exec 167014 %rroom%
detach 167014 %rroom%
~
#167003
телепорт на предмете~
1 h 100
~
%echo% в шарике появилось небольшое отверстие, из которого хлынула  энергия.
%send% %actor% высвобождённая энергия закружилась в вихре, образуя портал, в который стремительно затянуло вас и ваших согруппников
%echoaround% %actor% высвобождённая энергия закружилась в вихре, образуя портал, в который стремительно затянуло %actor.vname% и группу этого человека
%teleport% %actor% 166022
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 166022
done
eval gopa %actor.group%
%echoaround% %gopa% с характерным пуфф открылся портал, из которого появились:
foreach i %gopa%
  %echoaround% %i% %i.name%
  %send% %i% выпустив вас и остальную группу портал закрылся
done
%echo% после этого портал закрылся, а затраченная на его создание энергия втянулась обратно в шарик
~
#167004
дух опыта~
1 c 2
поглотить~
if !%arg.contains(сферу)%
  %send% %actor% я понимаю, что вы ultra_poglotitel_9000, но поглощать надо что-то, а не просто так
  halt
end
%send% %actor% вы всей душой пожелали поглотить сферу опыта, у вас это почти получилось, но вдруг из сферы вырвался светящийся дух. сфера при этом разлетелась на маленькие кусочки, которые мгновенно растаяли в воздухе
%echoaround% %actor% %actor.name% почти поглотил%actor.q% сферу опыта, но вдруг из неё вырвался светящийся дух. сама сфера при этом разлетелась на маленькие кусочки, которые мгновенно растаяли
%load% mob 167004
%send% %actor% хотите опыта халявного? ну так заберите! желаем приятной игры!
%purge% %self%
~
#167005
установка вейта чару~
1 c 1
вейт~
eval arg1 %arg.car%
eval victim %arg1.id%
eval arg2 %arg.words(2)%
%send% %actor% вы поставили вейт %victim.dname% в %arg2% секунд
foreach i %victim.pc%
  if %i% == %victim%
    %send% %victim% вам показалось, что само небо спустилось и сдавила вас со всех сторон, не давая ничего сделать
    %victim.wait(%arg2%)%
  else
    %send% %i% %victim.name% внезапно замер%victim.q% на месте, нелепо открыв рот в немом удивлении
  done
end
~
#167006
бонус опыта~
1 c 2
разорвать~
if !%arg.contains(свиток)%
  %send% %actor% пасть порву, моргало выколю!!!111 а что разрывать то будем, начальник?
  halt
end
%send% %actor% разорвав свиток, вы почувствовали неукротимую тягу к знаниям и огромную скорость знаний этих усвоения
%echoaround% %actor% варварским образом %actor.name% разорвал %actor.q% свиток опыта
dg_affect %actor% множитель награда 3 1440 0
%purge% %self%
~
#167007
рандомный процентный урон на объекте кроме использовавшего ~
1 h 100
~
%send% %actor% склянка взрывается, повреждая всех кроме вас
eval gopa %actor.all%
foreach i %gopa%
  %echo% обрабатываю %i.name%
  if %i.dex% > 50 || %i.clan% == СЛ || %i% == %actor%
    %send% %i% нет дамаги
  else
    eval tmp %i.hitp% / 100
    eval tmp2 %tmp%*(%random.21% + 9)
    %echo% скелет хиты  %i.hitp%  итого %tmp2%
    %echo% есть дамага %i.name% %tmp2%
    %damage% %i% %tmp2%
    %send% %i% есть дамага
  end
done
~
#167008
вызов моба~
1 c 2
вызвать~
if !%arg.contains(помощника)%
  %send% %actor% приём приём. а кого вызывать то будем, шеф?
  halt
end
if %actor.attackers% != 0
  %send% %actor% только не в бою
else
  %send% %actor% Вы  призываете Помощника клана "Серая Лига" себе напомощь!
  %echoaround% %actor% %actor.name% вызвал%actor.g% Помощника клана "Серая Лига" себе напомощь!
  %load% mob 166023
end

~
#167009
пенту на кланренту~
0 d 100
домой~
if %actor.attackers% != 0
  %send% %actor% только не в бою
else
  гов домой хотите, ну что ж
  %echo% внезапно дух взвивается вверх и нараспев начинает читать заклинание.
  %echo% это продолжается  некоторое время, но потом он умолкает и опускается к земле
  wait 3
  %portal% 166022 2
  гов переход продержится всего лишь час, поэтому поспешите
  %echo% произнеся эти слова помощник медленно растаял в воздухе
  %send% %actor% какое-то неведомое чувство вам подсказало, что дух вернулся обратно в своё вместилище
  %purge% %self%
end
~
#167010
удалён за ненадобностью~
2 z 100
~
%portal% 166022 2
~
#167011
прыжок~
1 c 1
прыжок~
%send% %actor% вы изо всех сил подпрыгнули, и
if %arg% < -1
  %send% %actor% ничего не произошло
else
  %echoaround% %actor% %actor.name% исчез%actor.q%
  %teleport% %actor% %arg%
  %echoaround% %actor% %actor.name% появил%actor.u% из ниоткуда
end

~
#167012
вместилище~
0 d 1
помощник~
if %actor.attackers% != 0
  %send% %actor% только не в бою
else
  %load% obj 166023
  дать вместилище %actor.name%
end
~
#167013
отзыв духа~
0 c 2
отозвать~
if !%arg.contains(помощника)%
  %send% %actor% вы отозвали петицию в клан chaos. ой, не то! короче отзывать надо либо выйти поговорить, либо кого-то, это понятно?
  halt
end
%send% %actor% повинуясь вашему желанию, дух медленно растаял
%send% %actor% какое-то неведомое чувство вам подсказало, что дух вернулся обратно в своё вместилище
%echoaround% %actor% дух-помощник медленно растаял в воздухе
%purge% %self%
~
#167014
удар молнией~
2 z 100
~
foreach i %victim.all%
  if %i% == %victim.id%
    %send% %victim% над вашей головой сгустились огромные грозовые тучи, из которых %actor.name% ударил в вас еще более огромной шаровой молнией
    %damage% %victim% %dmg%
  else
    %send% %i% над вашей головой сгустились огромные грозовые тучи, из которых %actor.name% ударил в %victim.vname% шаровой молнией
  done
end

~
#167015
сфера хаоса~
1 h 100
~
wait 1
%send% %actor% сфера разбивается на мелкие кусочки, выпуская из себя хаос, который направляется к вашим врагам
%echoaround% %actor% %actor.name% разбил%actor.q% сферу, из которой хлынул хаос
eval gopa %actor.all%
foreach i %gopa%
  if %i% == %actor% || %i.leader% == %actor% || %i.name% == крупье
    %send% %i% хаос обходит вас стороной
    %echoaround% %actor% хаос обходит %actor.vname% стороной
  elseif %i.affected_by(защита_богов) == 1
    %send% %i% боги защитили вас от хаоса
  else
    eval tmp %random.1000%
    %send% %i% хаос касается вас, получено урона: %tmp%
    %echoaround% %i% хаос касается %i.rname%, нанесено урона: %tmp%
    %damage% %i% %tmp%
  end
done
%purge% %self%

~
#167016
фазы моба~
0 k 100
~
eval tmp %self.hitp%*100/%self.maxhitp%
%echo% текущий уровень жизни: %tmp% %
if %tmp% < 10 && %tmp1% != 1
  eval tmp1 1
  global tmp1
  %echo% дух хаоса исчез
  %portal% 167011 20
  %teleport% %self% 167010
  world.zreset(1670)
end
if %tmp% < 25 && %tmp2% != 1
  eval tmp2 1
  global tmp2
  гов а что вы скажете на это, пыль под ногами?
  dg_cast 'разбитые' %self%
  dg_affect %self% сила защита_богов 1 6 1
end
if %tmp% < 50 && %tmp3% != 1
  eval tmp3 1
  global tmp3
  гов хотите печеньку?
  ухм
  %load% obj 167025
  %load% obj 167025
  %load% obj 167025
  бросить все.сфер
end
if %tmp% < 75 && %tmp4% != 1
  eval tmp4 1
  global tmp4
  гов все вы- жалкие рабы, именно поэтому вы подчинитесь мне прямо сейчас и замрёте, как уровень вашего развития
  dg_cast 'массовое оцепенение' %self.fighting%
end
~
#167017
переход на квест 1~
2 d 1
готовы~
%send% %actor% сразу после того, как вы сказали это слово, вас затянуло во внезапно появившуюся воронку, и через мгновение вы были уже совсем в другом месте
%echoaround% %actor% Сразу же после того, как %actor.name% произнес%actor.q% это слово, вас затянуло во внезапно появившуюся воронку, а через мгновение вы оказались совершенно в другом месте.
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167012
done
end
~
#167018
зелье проклятий~
1 i 100
~
eval gopa %actor.all%
foreach i %gopa%
  if %i% == %actor%
    %send% %actor% вы, размахнувшись, метнули склянку с зельем проклятий в %victim.vname%
  elseif %i% == %victim%
    %send% %victim% размахнувшись, %actor.name% метнул%actor.g% в вас склянку с каким-то зельем, и она разбилась прямо о вас, накладывая вредные аффекты
  else
    %send% %i% размахнувшись, %actor.name% метнул%actor.g% в %victim.vname% склянку с каким-то зельем
  end
done
dg_cast 'оцеп' %victim%
dg_cast 'длит глух' %victim%
dg_cast 'длит слепот' %victim%
dg_cast 'длит молч' %victim%
dg_cast 'яд' %victim%
%damage% %victim% 100
return 0
%purge% %self%

~
#167019
телепорт на куклу~
2 d 1
готово~
if %actor.haveobj(167033)% == 0
  %send% %actor% стыдно должно быть, я то вас никогда не обманывал
  halt
end
%echo% вдруг вас куда-то неожиданно потянуло, и через мгновение вы были уже не здесь
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167013
done

~
#167020
удалён за ненадобностью~
2 z 100
~
%portal% 167011 20
~
#167021
приветствуем старца~
0 d 1
привет приветствую приветствуем здравствуйте здорово хай~
гов ну здравствуйте, коль пришли. выслушайте ка, что я вам расскажу, раз гости моего дома.
%echo% старец задумался, закачался из стороны в сторону, завыл, и вы поняли , что именно в этот момент он окончательно обезумел.
%echo% но вдруг всё это прекратилось, и он заговорил, только немного другим голосом
гов помню я ко мне один чародей заглядывал, небылицы всякие рассказывал.
гов припоминаю, говорил что-то про рычаг, который дёрнешь- и издохнешь сразу. эка небылица! как же это так: дёрнешь- и помер.
гов меня вот волки даже кусали бывало, так и то зажило, не помер ведь, а тут рычаг.
гов да еще слово какое чудное- рычаг. не-нашинское оно, ой не-нашинское.!
гов или вот припоминаю, про куклу рассказывал: будто бы попросишь её о чём угодно, а она тебя даже за 3 версты услышит, да выполнит просьбу, если оно ей подсилу. как же это так, а? кукла она неживая, предмет бездушный!
гов а когда уходил этот прохвост, лицо у него подозрительное какое-то, загадку мне задал, так я и ответ до сих пор не узнал, может вы мне поможете?
attach 167022 %self%
detach 167021 %self%
~
#167022
загадка чародея~
0 d 1
да помогу поможем~
улыб
гов вот что он у меня спросил:
гов жил да был один крестьянин, долго жил, зим эдак сорок, и ещё половину от того, да горя не знал. напали на него два разбойника лихих, всю живность перебили.
гов а живности той было петуха 3 да курей 5, а потом ещё сказали ему злобно те душегубцы:
гов ты, дед, нам теперь мзду должен платить, а то не посмотрим, что старый, как есть поколотим. а должен ты нам каждые лун эдак 8 платить куны.
гов и спросил меня он так гаденько ухмыляясь: а сколько то кун должен крестьянин отдавать и ушёл, гад этакий.
wait 15s
attach 167023 %self%
выть
кричать сколько???
%echo% старец со всего разбегу врезался головой в стену избы
%damage% %self% 20
wait 10s
кричать признавайтесь!!!
%echo% старец продолжил биться головой о стену, на которой уже начали появляться кровавые разводы
%damage% %self% 20
wait 10s
кричать признавайтесь, ироды, сколько кун?
%echo% дед не останавливался, и на пол уже натекла приличная алая лужа
%damage% %self% 20
wait 10s
крич ну скажите, молю, сколько кун?
%echo% уже едва стоя на ногах, дед продолжил своё нелицеприятное занятие
%damage% %self% 20
wait 10s
%echo% не в силах вымолвить и слова, дед просто завывал на высокой ноте, но вдруг у него будто открылось второе дыхание, он вскочил, разбежался и в последний раз ударил головой о стену, раскалывая свой череп на части
%damage% %self% 10000

~
#167023
ответ на загадку старца~
0 d 1
тринадцать 13~
%echo% услышав это, дед внезапно замер, его лицо просветлело, он воскликнул:
%echo% спасибо вам, спасибо, спасибо!!! вы даже не представляете, как выручили старика, теперь я точно смогу уйти на покой. возьмите вот, думаю ценная вещица.
detach 167022 %self%
%load% obj 167033
дать буква %actor.name%
attach 167021 %self%
detach 167023 %self%

~
#167024
триг для куклы~
2 c 1
попросить~
calcuid tmpmob 167013 mob
eval tmparg %arg%
if !%arg%
  %send% %actor% вы и сами не поняли, чего захотели попросить
else
  attach 167025 %tmpmob%
  run 167025 %tmpmob%
end
~
#167025
выполнение просьбы на кукле~
0 z 100
~
%send% %actor% вы попросили куклу сделать %tmparg%
%tmparg%

~
#167026
ловушка в квесте 2~
2 e 100
~
wait 1
%echo% а вас предупреждали между прочим
%echo% "Вы провалили ивент. Вы оказались слишком слабы и ничтожны для выполнения важнейшего задания, от которого зависела судьба мира.
%echo% Люди, еще недавно верившие в вас и молившиеся за вас, сейчас лишь презрительно отворачиваются, не желая видеть ваше унижение".
foreach i %self.pc%
  %damage% %i% 10000
done
~
#167027
просмотр комнаты через предмет (усовершенствованный вариант)~
1 c 4
покатить~
if !%arg.contains(яблочко)%
  %send% %actor% да я то непротив, начальник, но в аргументах команды сейчас хоть шаром покати, каламбур однако
  halt
end
%send% %actor% вы покатили яблочком по блюдечку, появилась рябь, а потом вы увидели:
%echoaround% %actor% %actor.name% покатил%actor.q% яблочко по блюдечку
%actor.room(167014)%
%force% %actor% см
%actor.room(167013)%
~
#167028
переход на квест 3~
2 d 1
готово~
if %actor.haveobj(167034)% == 0
  %send% %actor% стыдно должно быть, я то вас никогда не обманывал
  halt
end
%send% %actor% Как только вы это сказали, то почувствовали ощутимый пинок по заднему месту, а пока потирали синяк, образовавшийся от удара, обстановка вокруг вас была уже совсем другой.
%echoaround% %actor% Как только %actor.name% сказал%actor.q% это, вы почувствовали ощутимый пинок по заднему месту, а пока потирали образовавшийся синяк от удара, обстановка вокруг вас стала уже совсем другой
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167015
done

~
#167029
ловушка 1 на квесте 3~
2 g 100
~
wait 1
%echo% стены высасывают вашу энергию, и вы валитесь с ног от усталости
eval gopa %actor.pc%
foreach i %gopa%
  %i.move(1)%
done
eval tmp %random.2%
if %tmp% == 1
  foreach i %gopa%
    %damage% %i% 1000
    %send% %i% стены высасывают ваши жизненные силы, вы чувствуете себя значительно хуже
  end
done
~
#167030
вторая ловушка в квесте 3~
2 g 100
~
wait 1
%echo% стены высасывают вашу энергию, и вы валитесь с ног от усталости
eval gopa %actor.pc%
foreach i %gopa%
  %i.move(1)%
done
eval tmp %random.3%
if %tmp% == 1
  foreach i %gopa%
    %damage% %i% 1000
    %send% %i% стены высасывают ваши жизненные силы, вы чувствуете себя значительно хуже
  end
done
calcuid tmproom 167019 room
attach 167031 %tmproom%
~
#167031
рычаг~
2 c 100
дернуть~
if !%arg.contains(рычаг)%
  %send% %actor% цель для дёрганья не найдена, попробуйте еще раз
  halt
end
%send% %actor% Вы дёрнули рычаг, и раздался какой-то щелчёк. Больше ничего не произошло, абсолютно. Может стоит преступить к второму варианту?
wat 167015 %load% obj 167036
wat 167015 %echo% раздался щелчёк, и из отверстия трубы на землю что-то упало
wat 167015 %echo% голос сказал: Можете не ждать %actor.vname%, он%actor.g% уже не вернётся.
wat 167015 %echo% голос дьявольски захохотал, отдаваясь тупой болью в черепе
detach 167031 %self%

~
#167032
самоубийство~
2 c 1
самоубийство~
%send% %actor% наполнившись уверенностью, вы взяли из кучи нож по острее и резким движением вскрыли себе горло от уха до уха
%echo% "Вы провалили ивент. Вы оказались слишком слабы и ничтожны для выполнения важнейшего задания, от которого зависела судьба мира.
%echo% Люди, еще недавно верившие в вас и молившиеся за вас, сейчас лишь презрительно отворачиваются, не желая видеть ваше унижение".
%damage% %actor% 1999999999
~
#167033
переход на квест 4~
2 d 1
готово~
if %actor.haveobj(167036)% == 0
  %send% %actor% стыдно должно быть, я то вас никогда не обманывал
  halt
end
%echo% День поменялся местами с ночью, земля с небом. Ну или у вас галлюцинации и вы просто попали в другое место.
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167020
done

~
#167034
лоад буквы после барьера~
0 f 100
~
%echo% раздался хлопок и барьер исчез, показывая другую часть помещения, в которой что-то лежит
%load% obj 167035
%load% obj 167037
calcuid tmpobj 167035 obj
calcuid tmpuid 167037 obj
%tmpobj.put(%tmpuid%)%

~
#167035
телепорт на начало~
2 d 1
готово~
if %actor.haveobj(167035)% == 0
  %send% %actor% стыдно должно быть, я то вас никогда не обманывал
  halt
end
%echo% Как то даже обыденно вас просто подбросило вверх, а приземлились вы уже в помещении с алтарём.
calcuid tmpobj 167031 obj
wat 167011 %purge% %tmpobj%
wat 167011 %load% obj 167032
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167011
done

~
#167036
собрать слова~
2 c 1
собрать~
if %arg.contains(хаос)%
  eval tmpvar 1
elseif %arg.contains(соха)%
  eval tmpvar 2
else
  %echo% Ну что собирать будем?
  halt
end
eval list 167033 167034 167035 167036
eval gopa %actor.all%
eval count 0
if %tmpvar% == 1
  foreach i %gopa%
    foreach m %list%
      if %i.haveobjs(%m%)% !=0
        eval count %count% + 1
        calcuid tmp %m% obj
        %send% %actor% У вас есть %tmp.name%
        %purge% %tmp%
      end
    done
    %echo% всего букв %count%
  done
  if %count% == 4
    %send% %actor% как только вы выложили слово хаос, буквы соединились, образуя прямоугольник, который вырвался у вас из рук, подлетел к алтарю и идеально вместился в отверстие в нём
    %echoaround% %actor% как только %actor.name% выложил%actor.q% слово хаос, буквы соединились, образуя прямоугольник, который вырвался из рук этого человека, подлетел к алтарю и идеально вместился в отверстие в нём
    foreach i %gopa%
      %teleport% %i% 167010
    done
  end
end
if %tmpvar% == 2
  foreach i %gopa%
    foreach m %list%
      if %i.haveobjs(%m%)% !=0
        eval count %count% + 1
        calcuid tmp %m% obj
        %send% %actor% У вас есть %tmp.name%
        %purge% %tmp%
      end
    done
  done
  %echo% всего букв %count%
  if %count% == 4
    %echo% как только было выложено слово соха, алтарь засиял багровым светом, раздался ужасающий грохот, а через мгновение вас просто раздавило потолком, упавшим сверху
    %echo% "Вы провалили ивент. Вы оказались слишком слабы и ничтожны для выполнения важнейшего задания, от которого зависела судьба мира.
    %echo% Люди, еще недавно верившие в вас и молившиеся за вас, сейчас лишь презрительно отворачиваются, не желая видеть ваше унижение".
  done
  eval jgopa %actor.all%
  foreach j %jgopa%
    %damage% %j% 1000000
  done
end
end
~
#167037
смерть деда~
0 f 100
~
%echo% "Вы провалили ивент. Вы оказались слишком слабы и ничтожны для выполнения важнейшего задания, от которого зависела судьба мира.
%echo% Люди, еще недавно верившие в вас и молившиеся за вас, сейчас лишь презрительно отворачиваются, не желая видеть ваше унижение".
eval gopa %actor.all%
foreach i %gopa%
  eval tmp %i.loadroom%
  %teleport% %i% %tmp%
done

~
#167038
удалено за ненадобностью~
2 c 1
старт~

~
#167039
удалено за ненадобностью~
2 z 100
~

~
#167040
удалено за ненадобностью~
2 z 100
~

~
#167041
удалено за ненадобностью~
0 d 0
~

~
#167042
медальон~
1 c 3
пожелать~
if !%tmpcast%
  eval tmpcast 66
end
dg_cast %arg%
%send% %actor% вы пожелали создать заклинание %arg%
%echoaround% %actor% %actor.name% пожелал%actor.q% создать заклинание %arg%
eval tmpcast %tmpcast% -1
global tmpcast
%send% %actor% текущий уровень заряда: %tmpcast%
if %tmpcast% < 1
  %echo% медальон, напоследок вспыхнув, пропал
  %purge% %self%
end

~
#167043
способности для всеобуча~
1 c 2
успособность~
if !%tmpturn%
  eval tmpturn 13
end
%echo% %arg%
eval tmparg %arg%
if %actor.can_get_feat(%tmparg%)% == 0
  %echo% вы не можете изучить эту способность
else
  %featturn% %actor% %tmparg% set
  eval tmpturn %tmpturn% -1
  global tmpturn
  %echo% осталось зарядов: %tmpturn%
end
if %tmpturn% < 1
  %send% %actor% всеобуч лишился последней капли своей силы и рассыпался в ваших руках
  %purge% %self%
end

~
#167044
умения для всеобуча~
1 c 2
уумение~
if !%tmpturn%
  eval tmpturn 13
end
%echo% %arg%
eval tmparg %arg%
if %actor.can_get_skill(%tmparg%)% == 0
  %echo% вы не можете изучить это умение
else
  %skillturn% %actor% %tmparg% set
  eval tmpturn %tmpturn% -1
  global tmpturn
  %echo% осталось зарядов: %tmpturn%
end
if %tmpturn% < 1
  %send% %actor% всеобуч лишился последней капли своей силы и рассыпался в ваших руках
  %purge% %self%
end

~
#167045
улучшение умений для всеобуча~
1 c 2
удобавить~
if !%tmpturn%
  eval tmpturn 13
end
%echo% %arg%
eval tmparg %arg%
%echo% вы умение %tmparg%
%skilladd% %actor% %tmparg% 200
eval tmpturn %tmpturn% -1
global tmpturn
%echo% осталось зарядов: %tmpturn%
end
if %tmpturn% < 1
  %send% %actor% всеобуч лишился последней капли своей силы и рассыпался в ваших руках
  %purge% %self%
end
~
#167046
заклинания для всеобуча~
1 c 2
узаклинание~
if !%tmpturn%
  eval tmpturn 13
end
%echo% %arg%
eval tmparg %arg%
if %actor.can_get_spell(%tmparg%)% == 0
  %echo% вы не можете изучить это заклинание
else
  %spellturn% %actor% %tmparg% set
  eval tmpturn %tmpturn% -1
  global tmpturn
  %echo% осталось зарядов: %tmpturn%
end
if %tmpturn% < 1
  %send% %actor% всеобуч лишился последней капли своей силы и рассыпался в ваших руках
  %purge% %self%
end

~
#167047
частичка хаоса~
1 i 100
~
if !%tmpkill%
  eval tmpkill 6
end
%send% %actor% вы достали частичку хаоса, которая рванулась вперёд, поглощая жизненные силы %victim.rname%
%send% %victim% вам дали частичку хаоса, которая поглотила ваши жизненные силы
eval tmp %victim.hitp(1)%
%tmp%
eval tmpkill %tmpkill% -1
global tmpkill
%send% %actor% текущее количество зарядов: %tmpkill%
if %tmpkill% < 1
  %send% %actor% частичка хаоса растаяла пряммо в ваших руках
  %purge% %self%
end
return 0

~
#167048
заряд всеобуча~
1 c 2
узаряд~
%send% %actor% текущий уровень заряда: %tmpturn%

~
#167049
заряд частички хаоса~
1 c 2
чхзаряд~
%send% %actor% текущий уровень заряда: %tmpkill%

~
#167050
заряд медальона~
1 c 3
мзаряд~
%send% %actor% текущий уровень заряда: %tmpcast%

~
#167051
печать порядка~
1 c 3
печать~
if !%tmpseal%
  eval tmpseal 169
end
dg_cast 'запечатать' %actor%
eval tmpseal %tmpseal% -1
global tmpseal
%send% %actor% осталось зарядов: %tmpseal%
if %tmpseal% < 1
  %send% %actor% печать порядка рассыпалась на маленькие кусочки, которые через мгновение растаяли
  %purge% %self%
end

~
#167052
заряды печати~
1 c 3
пзаряд~
%echo% текущее количество зарядов: %tmpseal%

~
#167053
restore на чп~
1 i 100
~
if !%tmprestore%
  eval tmprestore 66
end
%send% %actor% вы достали частичку порядка, которая рванулась вперёд, даря %victim.dname% жизненные силы, бодрость, готовность к новым свершениям и возможно силу земли
%send% %victim% к вам рванулась частичка порядка, даруя жизненные силы, бодрость, готовность к новым свершениям, а возможно даже силу земли
%victim.restore%
eval tmprestore %tmprestore% -1
global tmprestore
if %tmprestore% < 1
  %send% %actor% напоследок вспыхнув, частичка порядка растаяла в ваших руках
  %purge% %self%
end
return 0

~
#167054
заряды чп~
1 c 3
чпзаряд~
%send% %actor% текущее количество зарядов: %tmprestore%

~
#167055
диспел на чж~
1 i 100
~
if !%tmpdispel%
  eval tmpdispel 66
end
%send% %actor% вы достали частичку жизни, она рванулась вперёд и окатила %victim.vname% волной белого света, смывая все аффекты
%send% %victim% к вам устремилась частичка жизни, которая окатила вас волной белого света, смывающей все наложенные на вас аффекты
%victim.dispel%
eval tmpdispel %tmpdispel% -1
global tmpdispel
%send% %actor% осталось зарядов: %tmpdispel%
if %tmpdispel% < 1
  %send% %actor% частичка жизни растаяла прямо в ваших руках
  %purge% %self%
end
return 0

~
#167056
заряды на чж~
1 c 3
чжзаряд~
%echo% текущее количество зарядов: %tmpdispel%

~
#167057
выдача наград~
0 d 1
награда награду выполнил достоен получить забрать дай отдай выигрыш приз~
eval tmp %random.6%
говорить вы и правда достойны награды, посмотрим, насколько вы удачливы
%echo% в воздухе появилась игровая шестигранная кость, которая, совершая различные кульбиты, плиземлилась на пол
%echo% на верхней её грани на этот раз оказалось число %tmp%
гов %tmp%, ну что же
if %tmp% == 1
  %load% obj 167027
  дать все %actor.name%
  гов а теперь прощайте
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% пропал%actor.q%
elseif %tmp% == 2
  %load% obj 167028
  бросить все
  %force% %actor% взять все
  гов а теперь прощайте
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% пропал%actor.q%
elseif %tmp% == 3
  %load% obj 167030
  дать все %actor.name%
  гов а теперь прощайте
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% пропал%actor.q%
elseif %tmp% == 4
  %load% obj 167042
  дать все %actor.name%
  гов а теперь прощайте
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% пропал%actor.q%
elseif %tmp% == 5
  %load% obj 167043
  бросить все
  %force% %actor% взять все
  гов а теперь прощайте
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% пропал%actor.q%
elseif %tmp% == 6
  %load% obj 167044
  бросить все
  %force% %actor% взять все
  гов а теперь прощайте
  %teleport% %actor% %actor.loadroom%
  %echo% %actor.name% пропал%actor.q%
end

~
#167058
переход в комнату наград~
0 f 100
~
%echo% как только был нанесён последний удар, на месте духа открылась воронка, которая утянула всех куда-то
%teleport% %actor% 167021
eval gopa %actor.group%
foreach i %gopa%
  %teleport% %i% 167021
done

~
#167059
установка позиции чару~
1 c 1
позиция~
eval arg1 %arg.car%
eval victim %arg1.id%
eval arg2 %arg.cdr%
set pos %arg2%
if %arg2% == умирает
  set pos 1
elseif %arg2% == без сознания
  set pos 2
elseif %arg2% == в обмороке
  set pos 3
elseif %arg2% == спит
  set pos 4
elseif %arg2% == отдыхает
  set pos 5
elseif %arg2% == сидит
  set pos 6
elseif %arg2% == сражается
  set pos 7
elseif %arg2% == стоит
  set pos 8
else
  %send% %actor% непонятная позиция, установлена стоит
  set pos 8
end
%victim.position(%pos%)%
%send% %actor% для %victim.rname% позиция установлена в %arg2% (%pos%)
if %pos% == 1
  %send% %victim% внезапно вы забились в предсмертной агонии
elseif %pos% == 2
  %send% %victim% внезапно из вас вышибло дух
elseif %pos% == 3
  %send% %victim% внезапно вы упали в обморок
elseif %pos% == 4
  %send% %victim% ни с того  ни с сего вы уснули
elseif %pos% == 5
  %send% %victim% внезапно для себя вы присели отдохнуть
elseif %pos% == 6
  %send% %victim% вдруг ваши ноги подкосились, и вы уселись на задницу
elseif %pos% == 7
  %send% %victim% внезапно вы стали отчаянно колотить воздух. странно, не правда ли?
elseif %pos% == 8
  %send% %victim% вы встали. просто встали. вот и все.
end

~
#167060
казино~
2 c 1
играть~
%send% %actor% доступные режимы
%send% %actor% куны:
%send% %actor% игратьхполтора ставка: при выигрыше ставка, увеличенная в полтора раза
%send% %actor% игратьх3 ставка: при выигрыше утроенная ставка
%send% %actor% игратьх10 ставка: при выигрыше удесятерённая ставка
%send% %actor% игратьх100 ставка: при выигрыше ставка увеличивается в сто раз
%send% %actor% кости ставка (антуражная): при выигрыше ставка, увеличенная в полтора раза
%send% %actor% автомат ставка: классический игровой фруктовый автомат, есть джекподы
%send% %actor% опыт:
%send% %actor% оиграть: ставка: при выигрыше ставка удваивается
%send% %actor% примечание: прибавляется опыт для капа одного получения опыта на уровне, кап этот можно узнать, убив монстра выша вас по уровню. отнимается проигранный опыт в полном размере
%send% %actor% приятной игры

~
#167061
игратьхполтора~
2 c 1
игратьхполтора~
if !%balans%
  eval balans 0
end
eval  rnd %random.10%
%send% %actor% ваша ставка: %arg% кун
if %actor.gold% < %arg%
  %echo% а у вас столько и нет, пойдите прочь, врунишка!
  %teleport% %actor% %actor.loadroom%
  halt
end
if %arg% < 1
  %send% %actor% вы чё, великим кулхацкером себя возомнили? а по заднице не хотите?
  halt
end
if %rnd% < 5
  eval casino %arg%*3/2
  eval casino2 %actor.gold(+%casino)%
  %send% %actor% вы выиграли %casino% кун
  %send% %actor% стало кун: %casino2% %actor.gold%
  wat 167023 %echo% %actor.name% выиграл%actor.g% %casino% кун
else
  %send% %actor% вы проиграли %arg% кун
  eval casino3 %actor.gold(-%arg%)%
  %send% %actor% осталось кун: %casino3% %actor.gold%
  wat 167023 %echo% %actor.name% проиграл%actor.g% %arg% кун
  eval balans %balans% +%arg%
  remote balans %self%
end
%send% %actor% ещё попробуем?

~
#167062
игратьх3~
2 c 1
игратьх3~
eval rnd %random.4%
%send% %actor% ваша ставка: %arg% кун
if %actor.gold% < %arg%
  %echo% а у вас столько и нет, пойдите прочь, врунишка!
  %teleport% %actor% %actor.loadroom%
  halt
end
if %arg% < 1
  %send% %actor% вы чё, великим кулхацкером себя возомнили? а по заднице не хотите?
  halt
end
if %rnd% == 1
  eval casino %arg%*3
  eval casino2 %actor.gold(+%casino)%
  %send% %actor% вы выиграли %casino% кун
  %send% %actor% стало кун: %casino2% %actor.gold%
  wat 167023 %echo% %actor.name% выиграл%actor.g% %casino% кун
else
  %send% %actor% вы проиграли %arg% кун
  eval casino3 %actor.gold(-%arg%)%
  %send% %actor% осталось кун: %casino3% %actor.gold%
  wat 167023 %echo% %actor.name% проиграл%actor.g% %arg% кун
  eval balans %balans% +%arg%
  global balans
end
%send% %actor% ещё попробуем?

~
#167063
игратьх10~
2 c 1
игратьх10~
eval rnd %random.11%
%send% %actor% ваша ставка: %arg% кун
if %actor.gold% < %arg%
  %echo% а у вас столько и нет, пойдите прочь, врунишка!
  %teleport% %actor% %actor.loadroom%
  halt
end
if %arg% < 1
  %send% %actor% вы чё, великим кулхацкером себя возомнили? а по заднице не хотите?
  halt
end
if %rnd% == 1
  eval casino %arg%*10
  eval casino2 %actor.gold(+%casino)%
  %send% %actor% вы выиграли %casino% кун
  %send% %actor% стало кун: %casino2% %actor.gold%
  wat 167023 %echo% %actor.name% выиграл%actor.g% %casino% кун
else
  %send% %actor% вы проиграли %arg% кун
  eval casino3 %actor.gold(-%arg%)%
  %send% %actor% осталось кун: %casino3% %actor.gold%
  wat 167023 %echo% %actor.name% проиграл%actor.g% %arg% кун
  eval balans %balans% +%arg%
  global balans
end
%send% %actor% ещё попробуем?

~
#167064
игратьх100~
2 c 1
игратьх100~
eval rnd %random.100%
%send% %actor% ваша ставка: %arg% кун
if %actor.gold% < %arg%
  %echo% а у вас столько и нет, пойдите прочь, врунишка!
  %teleport% %actor% %actor.loadroom%
  halt
end
if %arg% < 1
  %send% %actor% вы чё, великим кулхацкером себя возомнили? а по заднице не хотите?
  halt
end
if %rnd% == 1
  eval casino %arg%*100
  eval casino2 %actor.gold(+%casino)%
  %send% %actor% вы выиграли %casino% кун
  %send% %actor% стало кун: %casino2% %actor.gold%
  wat 167023 %echo% %actor.name% выиграл%actor.g% %casino% кун
else
  %send% %actor% вы проиграли %arg% кун
  eval casino3 %actor.gold(-%arg%)%
  %send% %actor% осталось кун: %casino3% %actor.gold%
  wat 167023 %echo% %actor.name% проиграл%actor.g% %arg% кун
  eval balans %balans% +%arg%
  global balans
end
%send% %actor% ещё попробуем?

~
#167065
кости~
2 c 1
кости~
%send% %actor% вы решили сыграть в кости на %arg% кун и подошли к одному из столов, на котором лежали два шестигранных кубика
if %arg% < 1
  %send% %actor% невидимая рука ощутимо стукнула вас по затылку и вам стало очень стыдно за желание обмануть игорный дом
  halt
end
eval multy %random.6%
eval multy2 %random.6%
%send% %actor% вы взяли один из кубиков, потрясли его в ладонях и бросили на стол: выпало %multy%
%send% %actor% второй кубик взлетел в воздух, перекувыркнулся пару раз и опустился на стол: выпало %multy2%
if %actor.gold% < %arg%
  %send% %actor% вдруг вы поняли, что у вас нет той суммы, на которую вы самонадеянно решили играть
  %send% %actor% вам захотелось провалиться под землю от стыда, так оно и случилось
  %teleport% %actor% %actor.loadroom%
  halt
end
if %multy% == %multy2%
  %send% %actor% ничья: поняли вы
elseif %multy% > %multy2%
  eval multy3 %arg%*3/2
  eval multy4 %actor.gold(+%multy3%)%
  %send% %actor% вы выиграли %multy3% кун
  %send% %actor% стало кун: %multy4% %actor.gold%
  wat 167023 %echo% %actor.name% выиграл%actor.g% %multy3% кун
elseif %multy% < %multy2%
  eval multy5 %actor.gold(-%arg%)%
  %send% %actor% вы проиграли %arg% кун
  %send% %actor% осталось кун: %multy5% %actor.gold%
  wat 167023 %echo% %actor.name% проиграл%actor.g% %arg% кун
  eval balans %balans% +%arg%
  global balans
end

~
#167066
игратьх2 на опыт~
2 c 1
оиграть~
if !%obalans%
  eval obalans 0
end
eval rnd %random.3%
%send% %actor% ваша ставка: %arg% опыта
if %actor.exp% < %arg%
  %send% %actor% а у вас столько и нет, пойдите прочь, врунишка!
  %teleport% %actor% %actor.loadroom%
  halt
end
if %arg% < 1
  %send% %actor% вы чё, великим кулхацкером себя возомнили? а по заднице не хотите?
  halt
end
if %rnd% == 1
  eval casino %arg%*2
  eval casino2 %actor.exp(+%casino)%
  %send% %actor% вы выиграли %casino% опыта
  %send% %actor% стало опыта: %casino2% %actor.exp%
  wat 167023 %echo% %actor.name% выиграл%actor.g% %casino% опыта
else
  eval casino3 %actor.exp(-%arg%)%
  %send% %actor% вы проиграли %arg% опыта
  %send% %actor% стало опыта: %casino3% %actor.exp%
  wat 167023 %echo% %actor.name% проиграл%actor.g% %arg% опыта
  eval obalans %obalans% +%arg%
  remote obalans %self%
end
%send% %actor% ещё попробуем?

~
#167067
магазин в казино~
2 c 100
купить~
calcuid krupye 167005 mob
if %arg.contains(билет)%
  if %actor.gold% < 1500000
    %send% %actor% вот как накопите необходимую сумму, так и покупайте
  else
    %send% %actor% вы изъявили желание купить билет, протянули крупье заранее приготовленные деньги
    %actor.gold(-1500000)%
    exec 167069 %krupye%
    %echoaround% %actor% %actor.name% приобрел%actor.g% билет фартуны
    eval balans %balans% +1500000
    global balans
  end
elseif %arg.contains(кость)%
  if %actor.gold% < 10000
    %send% %actor% вы не в состоянии купить это, а взаймы здесь не дают
  else
    %send% %actor% вы решили купить заколдованную игральную кость и протянули крупье деньги
    %actor.gold(-10000)%
    exec 167069 %krupye%
    %echoaround% %actor% %actor.name% приобрел%actor.g% заколдованную игральную кость
    eval balans %balans% +10000
    global balans
  end
else
  %send% %actor% здесь можно купить:
  %send% %actor% билет фартуны- 1500000 кун (купить билет)
  %send% %actor% заколдованная игральная кость- 10000 кун (купить кость)
end

~
#167068
колесо фартуны~
2 c 1
крутить~
eval wheel %random.52%
if !%arg.contains(колесо)%
  %send% %actor% а чё крутить то, папироску что ли?
  halt
end
if %actor.haveobj(167040) == 0
  %send% %actor% для этого нужен билет фартуны, которого у вас нема
  halt
else
  %send% %actor% шарик побежал по ячейкам на колесе, ускорился, начал притормаживать и остановился на цифре %wheel%
  calcuid wheelobj 167040 obj
  %purge% %wheelobj%
end
if %wheel% < 27
  %send% %actor% вы ничего не выиграли, ну как же так!
  %echoaround% %actor% %actor.name% покрутил%actor.g% колесо фартуны и ничего не выиграл%actor.g%. какое разочарование!
  wat 167023 %echo% %actor.name% покрутил%actor.g% колесо фартуны и ничего не выиграл%actor.g%. какое разочарование!
elseif %wheel% > 26
  eval list 92104 166113 166114 167025 167041 167045 167046 167047 167048 167049 167018 167065 167067 167068 167069 167070 167071 167072 167073 167074 167075 167076 167077 167078 set60 set61
  eval wheelarray %array.item(%list%, %random.26%)%
  if %wheelarray% == set60
    eval list 167079 167080 167081 167082 167083 167084 167085 167086 167087 167088
    eval wheelarray %array.item(%list%, %random.10%)%
    %load% obj %wheelarray%
    calcuid wheelobj %wheelarray% obj
    global wheelarray
  elseif %wheelarray% == set61
    eval list 166115 166116 166117 166118 166119 166120 166121 166122 166123 166124 166125 166126 166127
    eval wheelarray %array.item(%list%, %random.12%)%
    %load% obj %wheelarray%
    calcuid wheelobj %wheelarray% obj
    global wheelobj
  else
    %load% obj %wheelarray%
    calcuid wheelobj %wheelarray% obj
  end
  %send% %actor% вы выиграли %wheelobj.vname%
  %force% %actor% взять %wheelobj.name%
  %echoaround% %actor% %actor.name% покрутил%actor.g% колесо фартуны и выиграл%actor.g% %wheelobj.vname%
  wat 167023 %echo% %actor.name% покрутил%actor.g% колесо фартуны и выиграл%actor.g% %wheelobj.vname%
end

~
#167069
крупье дает купленный билет~
0 z 100
~
if %arg.contains(билет)%
  гов вот, держите
  %load% obj 167040
  дать билет %actor.name%
elseif %arg.contains(кость)%
  гов пожалуйста
  %load% obj 167092
  дать кость %actor.name%
end

~
#167070
ресет зоны 1670~
2 f 100
~

~
#167071
билеты у крупье~
0 d 1
билет~
eval utime %date.unix%
eval time1 %actor.getquest(167000)%
eval time2 (%utime% -%time1%)/60
if %time2% < 60
  eval time3 60 -%time2%
  гов не дам я тебе пока билета, подходи через %time3% минут
  миг %actor.name%
  шеп %actor.name% но я могу продать тебе этот билет, всего лишь миллион кун да еще половина от того
else
  гов билет, секундочку
  %send% %actor% %self.name% крикнул%actor.g% что-то, открылась незаметная на вид дверка, из которой кто-то выбежал и сунул ему в руки билет
  гов а вот и ваш билет, держите
  %load% obj 167040
  дать билет %actor.name%
  %actor.setquest(167000 %date.unix%)%
end

~
#167072
ор про билеты~
0 n 100
~
ор в игорном доме Дариана, что в киеве, снова появились билеты фартуны, успейте получить один из них и попытать удачу

~
#167073
сейф с кодовым замком~
1 c 4
ввести~
if !%rnd%
  eval rnd %random.999999999%
  global rnd
end
if !%try%
  eval try 66
end
%send% %actor% вы решили попытаться ввести код от сейфа и набрали %arg%
eval try %try% -1
global try
if %try% < 1
  %send% %actor% вдруг сейф начал сжиматься в маленькую точку, а затем и вовсе исчез, кажется вы не справились
  %purge% %self%
  halt
end
if %arg% < %rnd%
  %send% %actor% мало: стало понятно вам
elseif %arg% > %rnd%
  %send% %actor% многовато: подсказало вам неведомое чувство
end
%send% %actor% осталось попыток: %try%
if %arg% == %rnd%
  %send% %actor% некоторое время ничего не происходило, но вдруг с щелчком сейф резко раскрылся, роняя всё своё содержимое на пол
  %purge% %self%
end

~
#167074
отзыв тучи~
0 c 100
хлопнуть~
%send% %actor% вы хлопнули в ладоши, и грозовая туча начала быстро уменьшаться, а затем подлетела и впиталась в вашу руку
if %actor.sex% == 1
  %echoaround% %actor% %actor.name% хлопнул в ладоши, грозовая туча рядом с ним начала сжиматься, а затем подлетела к нему и впиталась в его руку
elseif %actor.sex% == 2
  %echoaround% %actor% %actor.name% хлопнула в ладоши, грозовая туча рядом с ней начала уменьшаться, а затем подлетела к ней и впиталась в ее руку
end
%purge% %self%

~
#167075
призыв маунта~
1 c 1
топнуть~
%send% %actor% вы топнули ногой, и перед вами появилась огромная грозовая туча
%load% mob 167007
%echoaround% %actor% %actor.name% топнул, и рядом с ним появилась огромная грозовая туча
calcuid tucha 167007 mob
attach 167074 %tucha%

~
#167076
камень возрождения~
1 c 3
привязка~
%send% %actor% повинуясь вашему желанию, камень возрождения привязал ваши души к этому месту
%actor.loadroom(%actor.realroom%)%
eval gopa %actor.group%
foreach i %gopa%
  %send% %i% повенуясь желанию %actor.rname%, камень возрождения привязал ваши души к этому месту
done
foreach i %gopa%
  %i.loadroom(%actor.realroom%)%
done
%purge% %self%

~
#167077
дезинтегратор~
1 c 3
направить~
eval victim %arg.id%
eval victimvnum %arg.vnum%
%echo% имя: %victim.name%, ID: %victim%, VNum: %victimvnum%, UID: %victim.uniq%
if %victim% == %actor% && %actor.name% != дариан
  %send% %actor% дезинтеграция владельца, операция невозможна.
  halt
end
if %victim.realroom% != %actor.realroom% && %actor.name% != Дариан
  %echo% цель не найдена, повторите попытку
  halt
end
eval victimrealroom %victim.realroom%
if %victimrealroom% < 1
  %send% %actor% такой цели не существует, повторите попытку.
  halt
end
%send% %victim% появившийся из неизвестности луч дезинтегрировал вас!!!
eval goldnow %victim.gold%
eval gold %victim.gold(-%goldnow%)%
%echo% куны с собой. поиск. найдено %goldnow%. дезинтеграция, осталось %gold%
eval banknow %victim.bank%
eval bank %victim.bank(-%banknow%)%
%echo% куны в банке. поиск. найдено %banknow%. дезинтеграция, осталось %bank%
eval hryvnnow %victim.hryvn%
eval hryvn %victim.hryvn(-%hryvnnow%)%
%echo% гривны. поиск. найдено %hryvnnow%. дезинтеграция, осталось %hryvn%
eval expnow %victim.exp% -1
eval exp %victim.exp(-%expnow%)%
%echo% накопленный опыт. поиск. найдено %expnow%. дезинтеграция, осталось %exp% %victim.exp%
%echo% вещи. поиск.
foreach i %victim.objs%
  %echo% обнаружено. %i.name%. дезинтеграция.
  %purge% %i%
done
%echo% экипировка. поиск. изъятие.
%force% %victim% снять все
foreach i %victim.objs%
  %echo% обнаружено. %i.name%. дезинтеграция.
  %purge% %i%
done
eval dmg %victim.hitp% +15
%echo% оболочка. поиск. найдено. структурных единиц: %dmg%. дезинтеграция.
eval renta %victim.loadroom(4056)%
%echo% изменение точки возраждения. успешно. текущая локация: %renta% %victim.loadroom%.
%damage% %victim% %dmg%
%echo% дезинтеграция прошла успешно.

~
#167078
скатерть самобранка~
1 c 7
пожелать~
eval object %arg.id%
eval objectvnum %object.vnum%
%echo% object = %object%, objectvnum = %objectvnum%
if %object.type% == 17 || %object.type% == 19
  %send% %actor% вы пожелали, чтоб создала вам скатерть-самобранка %arg.vname%, и чудо свершилось
  %load% obj %objectvnum%
  %echoaround% %actor% создала скатерьт самобранка %arg.vname%
else
  %send% %actor% это явно не еда
end

~
#167079
анализ персонажа~
1 c 1
анализ~
eval victim %arg.id%
if %victim.realroom% < 1
  %send% %actor% цель не найдена
  halt
end
%send% %actor% цель найдена, анализ завершен:
%send% %actor% ID: %victim%, VNum: %victim.vnum%, UID: %victim.uniq%
%send% %actor% имя: %victim.name% %victim.rname% %victim.dname% %victim.vname% %victim.tname% %victim.pname%
%send% %actor% пол:
if %victim.sex% == 1
  %send% %actor% мужской
elseif %victim.sex% == 2
  %send% %actor% женский
elseif %victim.sex% == 0
  %send% %actor% средний
elseif %victim.sex% == 3
  %send% %actor% множественное число
else
  %send% %actor% не определено
end
%send% %actor% возраст: %victim.age% лет
%send% %actor% вероисповедание:
if %victim.religion% == 0
  %send% %actor% язычество
elseif %victim.religion% == 1
  %send% %actor% христианство
else
  %send% %actor% не определено
end
%send% %actor% род:
if %victim.race% == 0
  %send% %actor% северяне
elseif %victim.race% == 1
  %send% %actor% поляне
elseif %victim.race% == 2
  %send% %actor% кривичи
elseif %victim.race% == 3
  %send% %actor% вятичи
elseif %victim.race% == 4
  %send% %actor% веляне
elseif %victim.race% == 5
  %send% %actor% древляне
else
  %send% %actor% не определено
end
if %victim.clan% == 0
  %send% %actor% в клане не состоит
else
  %send% %actor% состоит в клане: %victim.clan%
end
%send% %actor% склонность к свету/тьме: %victim.align%
%send% %actor% уровень развития: %victim.level%
%send% %actor% накопленный опыт: %victim.exp%
%send% %actor% количество перевоплощений: %victim.remort%
%send% %actor% класс персонажа:
if %victim.class% == 0
  %send% %actor% лекарь
elseif %victim.class% == 1
  %send% %actor% колдун
elseif %victim.class% == 2
  %send% %actor% тать
elseif %victim.class% == 3
  %send% %actor% богатырь
elseif %victim.class% == 4
  %send% %actor% наемник
elseif %victim.class% == 5
  %send% %actor% дружинник
elseif %victim.class% == 6
  %send% %actor% кудесник
elseif %victim.class% == 7
  %send% %actor% волшебник
elseif %victim.class% == 8
  %send% %actor% чернокнижник
elseif %victim.class% == 9
  %send% %actor% витязь
elseif %victim.class% == 10
  %send% %actor% охотник
elseif %victim.class% == 11
  %send% %actor% кузнец
elseif %victim.class% == 12
  %send% %actor% купец
elseif %victim.class% == 13
  %send% %actor% волхв
else
  %send% %actor% не определено
end
%send% %actor% жизнь: %victim.hitp%/%victim.maxhitp%
%send% %actor% мана: %victim.mana%/%victim.maxmana%
%send% %actor% энергия: %victim.move%/%victim.maxmove%
%send% %actor% основные параметры, врожденные +добавочные:
%send% %actor% сила: %victim.str% + %victim.stradd%
%send% %actor% ум: %victim.int% + %victim.intadd%
%send% %actor% мудрость: %victim.wis% + %victim.wisadd%
%send% %actor% подвижность: %victim.dex% + %victim.dexadd%
%send% %actor% телосложение: %victim.con% + %victim.conadd%
%send% %actor% обаяние: %victim.cha% + %victim.chaadd%
%send% %actor% уровень защиты, АС: %victim.acbase% + %victim.acadd%
%send% %actor% хитроллы: %victim.hr%
%send% %actor% дамроллы: %victim.dr%
%send% %actor% колдовство: %victim.castsucc%
%send% %actor% удача: %victim.morale%
%send% %actor% инициатива: %victim.initiative%
%send% %actor% яд: %victim.poison%
%send% %actor% размер: %victim.size% + %victim.sizeadd%
%send% %actor% вес: %victim.weight%
%send% %actor% переносимый вес: %victim.carry_weight%/%victim.can_carry_weight%
%send% %actor% экипировка:
set num 0
while %num% <= 18
  eval eq %victim.eq(%num%)%
  if %eq% == 0
    *nop
  else
    %send% %actor% %eq.name%
  end
  eval num %num% + 1
done
%send% %actor% предметы в инвентаре:
foreach i %victim.objs%
  %send% %actor% %i.iname%
done
eval victimrroom %victim.realroom%
eval victimlroom %victim.loadroom%
%send% %actor% сейчас находится: %victimrroom.name% ( %victimrroom% )
if %victim.fighting% == 0
  %send% %actor% сейчас не сражается
else
  eval fighting %actor.fighting%
  %send% %actor% сейчас сражается с %fighting.tname%
end
%send% %actor% рента: %victimlroom.name% ( %victimlroom% )
if %victim.rentable% > 0
  %send% %actor% может уйти на постой
else
  %send% %actor% не может уйти на постой
end
if %victim.is_killer% > 0
  %send% %actor% душегуб
else
  %send% %actor% не душегуб
end
if %victim.is_thief% > 0
  %send% %actor% вор
else
  %send% %actor% не вор
end
if %victim.agressor% < 1
  %send% %actor% не участвует в боевых действиях
else
  eval victimaroom %victim.agressor%
  %send% %actor% участвует в боевых действиях, место нападения: %victimaroom.name% ( %victimaroom% )
end

~
#167080
кастомный портал~
1 c 1
портал~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
if %arg1% == 0
  %send% %actor% портал <внум комнаты> <время действия в рл минутах>
  %send% %actor% чтоб убрать портал, во времени действия надо поставить 0
  halt
end
if %world.room(%arg1%)% == 0
  %send% %actor% такой комнаты не существует
  halt
end
%portal% %arg1% %arg2%
oat %arg1% %portal% %actor.realroom% %arg2%

~
#167081
открытие кастомного портала~
2 z 100
~
%portal% %arg%
wat %arg1% %portal% %actor.realroom% %arg2%
calcuid tmproom2 %arg1% room
detach 167081 %tmproom2%

~
#167082
стоп времени~
1 c 3
стоп~
eval timestoper %actor%
%send% %actor% вы щелкнули пальцами, и все вокруг замерло
%echoaround% %actor% %actor.name% щелкнул%actor.g% пальцами, всё вокруг замерло и вы в том числе
calcuid tmproom %actor.realroom% room
attach 167083 %tmproom%
attach 167084 %tmproom%
foreach i %actor.all%
  if %i% == %actor%
    halt
  else
    dg_affect %i% оцепенение оцепенение 1 26 7
  done
end
run 167084 %tmproom%

~
#167083
печать остановки времени~
2 eg 100
~
%send% %actor% вы попытались пройти дальше, но неведомая сила не дала вам этого сделать
%echoaround% %actor% %actor.name% попробовал%actor.g% пройти дальше, но ничего из этого путного не вышло
return 0

~
#167084
холд в остановке времени~
2 abz 100
~
foreach i %actor.all%
  if %i% == %timestoper%
  else
    dg affect %i% обездвижен оцепенение 1 26 7
  end
done

~
#167085
старт времени~
1 c 3
старт~
%send% %actor% вы щелкнули пальцами, и всё вокруг снова задвигалось
%echoaround% %actor% %actor.name% щелкнул%actor.g% пальцами, и вы наконец то почувствовали власть над своим телом
calcuid tmproom %actor.realroom% room
detach 167083 %tmproom%
detach 167084 %tmproom%

~
#167086
власть над миром~
1 c 3
/~
%arg%

~
#167087
зонэхо на предмете~
1 c 3
вывести~
eval arg1 %arg%
calcuid tmproom %actor.realroom% room
attach 167088 %tmproom%
exec 167088 %tmproom%
detach 167088 %tmproom%

~
#167088
зонэхо через exec~
2 z 100
~
wworldecho внезапно перед вами вспыхнули алые буквы:
wworldecho %arg1%

~
#167089
внепространственный карман всасывает~
1 c 3
всосать~
calcuid pocket 167060 obj
calcuid tmproom %actor.realroom% room
%send% %actor% внепространственный карман на вашем поясе открылся, всосал внутрь себя все окружающие предметы и закрылся.
%echoaround% %actor%  внепространственный карман на поясе %actor.rname% открылся, всосал в себя все окружающие предметы и снова закрылся
foreach i %tmproom.all%
  foreach k %tmproom.objects%
    %k.put(%pocket%)%
  done
  %force% %i% снять все
  foreach j %i.objs%
    %j.put(%pocket%)%
  done
done

~
#167090
телепортационный луч~
1 c 3
посылка~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval sendobjvnum %arg2.vnum%
calcuid sendobj %sendobjvnum% obj
%send% %actor% вы решили послать %victim.dname% %sendobj.vname% телепортационным лучом
if %victim% == %actor%
  %send% %actor% отправлять посылки самому себе? конгениально!
  halt
end
if %victim.vnum% != -1
  %send% %actor% отправлять посылки можно только игрокам
  halt
end
if %victim.haveobj(%self.vnum%)% == 0
  %send% %actor% %victim.dname% нечем принять ваш телепортационный луч
  halt
end
if %actor.haveobj(%sendobjvnum%)% == 0
  %send% %actor% но ведь у вас нету этой вещи!
  halt
else
  %purge% %sendobj%
  %load% obj %sendobjvnum%
  calcuid sendobj %sendobjvnum% obj
  %sendobj.put(%victim%)%
  %send% %actor% посылка успешно отправлена
  %send% %victim% по телепортационному лучу к вам пришла посылка от %actor.rname%
end

~
#167091
кастомный dg_affect~
1 c 3
акаст~
eval arg1 %arg.car%
eval arg2 %arg.words(2)%
eval arg3 %arg.words(3)%
eval arg4 %arg.words(4)%
eval arg5 %arg.words(5)%
eval arg6 %arg.words(6)%
eval victim %arg1.id%
dg_affect %victim% %arg2% %arg3% %arg4% %arg5% %arg6%
%send% %actor% вы махнули рукой в направлении %victim.rname%
%send% %victim% %actor.name% махнул рукой в вашу сторону, и вы почувствовали себя иначе
foreach i %actor.all%
  if %i% == %actor% || %i% == %victim%
    halt
  else
    %send% %i% %actor.name% махнул рукой в сторону %victim.rname%
  done
end

~
#167092
dg_affect на всех~
1 c 3
всеакаст~
eval arg1 %arg.car%
eval arg2 %arg.words(2)%
eval arg3 %arg.words(3)%
eval arg4 %arg.words(4)%
eval arg5 %arg.words(5)%
%send% %actor% вы вытянули руки вперед и развели их в стороны, указывая на всех, находящихся рядом
%echoaround% %actor% %actor.name% вытянул руки перед собой и развел их в стороны, указывая на всех вокруг, и вас в том числе, после чего вы почувствовали себя иначе
foreach i %actor.all%
  if %i% == %actor%
    halt
  else
    dg_affect %i% %arg1% %arg2% %arg3% %arg4% %arg5%
  done
end

~
#167093
кастомное изменение жизни~
1 c 1
жизнь~
set arg1 %arg.car%
set arg2 %arg.words(2)%
set arg3 %arg.words(3)%
eval victim %arg1.id%
%victim.hitp(%arg2%%arg3%)%
%send% %actor% у %victim.rname% жизнь установлена в %victim.hitp%

~
#167094
кастом мана~
1 c 3
мана~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval mp %victim.mana(%arg2%)%
%send% %actor% у %victim.rname% мана установлена в %mp%

~
#167095
кастом энергия~
1 c 3
энергия~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval ep %victim.move(%arg2%)%
%send% %actor% у %victim.rname% энергия установлена в %ep%

~
#167096
кастом каст~
1 c 3
колдовство~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval cast %victim.castsucc(%arg2%)%
%send% %actor% у %victim.rname% колдовство установлено в %cast%

~
#167097
кастом размер~
1 c 3
размер~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval size %victim.sizeadd(%arg2%)%
%send% %actor% у %victim.rname% размер установлен в %victim.size% врожденный + %size% добавочный

~
#167098
кастом флаг~
1 c 3
флаг~
set arg1 %arg.words(1)%
set arg2 %arg.words(2)%
calcuid tmproom %arg1% room
set flag %tmproom.flag(%arg2%)%
%echo% Для комнаты %arg1% делаем флаг %arg2% в %flag%

~
#167099
рента~
1 c 3
рента~
eval arg1 %arg.car%
eval arg2 %arg.cdr%
eval victim %arg1.id%
eval lroom %victim.loadroom(%arg2%)%
%send% %actor% у %victim.rname% комната ренты установлена в %lroom%

~
$
$
