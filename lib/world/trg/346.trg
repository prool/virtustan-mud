#34600
гриш помогу екзарху~
0 d 0
помогу спасу~
wait 2s
осм %actor.name%
ул %actor.name%
wait 1
say Смекалка твоя достойна восхищения!
say Этой зимой, во время угрского набега, украли у меня мой любимый антиминс.
say Это главный священный предмет храма....
say Без него службу вести нельзя...
пла
say Боже мой! Как бы я вознаградил нашедшего!!!!
пла
~
#34601
даем антимин екзарху~
0 j 100
~
wait 1
if %object.vnum% != 34604 
  say Зачем мне это? 
  eval getobject %object.name%
  брос %getobject.car%.%getobject.cdr%
  halt
end
wait 1
mpurge %object%
wait 2s
say Благодарю Тебя, Боже! Воистину велика сила Твоя!!
wait 2s
mecho Святой экзарх спрятал антиминс подальше.
if %world.curobjs(3314)% < 1 && %world.curobjs(3315)% < 1 && %random.3%  == 1
  mload obj 3314
  дать все .%actor.name%
else
  %actor.exp(+100000)%
  msend За доброе дело Вы получили 100000 очков опыта.
end
%actor.gold(+5000)%
msend Святой экзарх дал Вам 5000 кун.
mechoaround %actor% Святой экзарх дал большую груду кун %actor.dname%.
пока
~
#34602
входим в храм без санка~
0 r 100
~
if !(%actor.affect(освящение)%)
  say Ты, нечестив%actor.w% %actor.name%!!! Как посмел%actor.g% взойти на Престол!!!!
  встать
  mkill %actor%
end
~
#34603
заходиш к мытарю~
0 q 100
~
wait 1s
say Гой еси, добрый путник...
say Ты в деревне - во Сливяновке, а Галич-град на юго-западе.
say А Киев-стольный град далече на востоке будет.
say Места тута неспокойные, того и глядишь нагрянут злобные угры.
say С горем пополам от них обороняемся...
пла
calcuid repopmob 34612 mob
detach 34603 %repopmob.id%
~
#34604
репоп тригов в зоне~
2 f 100
~
calcuid rep1opmob 34612 mob
detach 34603 %rep1opmob.id%
attach 34603 %rep1opmob.id%
~
#34605
лоад набега - пока что как то так - набег~
0 c 0
набег гоу~
wait 2s
msend %actor% Вы произнесли тайную команду, вызывающую Угрский набег на Киев.
mechoaround %actor% %actor.name% вызвал набег Угров на Киев... Что-то случится ужасное....
switch %random.3%
  case 1
    wait 2s
    mat 34614 mecho Несколько угров-разведчиков, крадучись, пробралось сюда.
    mat 34614 mload mob 34632
    mat 34614 mload mob 34632
    mat 34614 mload mob 34632
    wait 40s
    mat 34614 mecho Передовой угрский отряд пришел сюда.
    mat 34614 mload mob 34631
    mat 34614 mload mob 34631
    mat 34614 mload mob 34631
    wait 40s
    mat 34614 mecho Один из угрских отрядов пришел сюда.
    mat 34614 mload mob 34630
    mat 34614 mload mob 34630
    mat 34614 mload mob 34630
    wait 40s
    mat 34614 mecho Отряд вооруженных до зубов угров пришел сюда.
    mat 34614 mload mob 34629
    mat 34614 mload mob 34629
    mat 34614 mload mob 34629
  break
  case 2
    wait 2s
    mat 34614 mecho Несколько угров-разведчиков, крадучись, пробралось сюда.
    mat 34614 mload mob 34632
    mat 34614 mload mob 34632
    mat 34614 mload mob 34632
    mat 34614 mload mob 34632
    mat 34614 mload mob 34632
    wait 60s
    mat 34614 mecho Передовой угрский отряд пришел сюда.
    mat 34614 mload mob 34631
    mat 34614 mload mob 34631
    mat 34614 mload mob 34631
    mat 34614 mload mob 34631
    mat 34614 mload mob 34631
    wait 60s
    mat 34614 mecho Один из угрских отрядов пришел сюда.
    mat 34614 mload mob 34630
    mat 34614 mload mob 34630
    mat 34614 mload mob 34630
    mat 34614 mload mob 34630
    mat 34614 mload mob 34630
    wait 60s
    mat 34614 mecho Отряд вооруженных до зубов угров пришел сюда.
    mat 34614 mload mob 34629
    mat 34614 mload mob 34629
    mat 34614 mload mob 34629
    mat 34614 mload mob 34629
    mat 34614 mload mob 34629
  break
  case 3
    wait 2s
    mat 34614 mecho Несколько угров-разведчиков, крадучись, пробралось сюда.
    mat 34614 mload mob 34632
    mat 34614 mload mob 34632
    wait 40s
    mat 34614 mecho Передовой угрский отряд пришел сюда.
    mat 34614 mload mob 34631
    mat 34614 mload mob 34631
    wait 40s
    mat 34614 mecho Один из угрских отрядов пришел сюда.
    mat 34614 mload mob 34630
    mat 34614 mload mob 34630
    wait 40s
    mat 34614 mecho Отряд вооруженных до зубов угров пришел сюда.
    mat 34614 mload mob 34629
    mat 34614 mload mob 34629
  break
  default
  break
done
end
~
#34606
угры подходят к гридю 34604~
0 h 100
~
wait 1
if %actor.vnum% == 34632
  mshou Други! Угры наступают на Киев! К оружию!
end
wait 1
if %actor.vnum% == 34631
  mshou Братья! Уграская орда наступает на Киев-град! За оружие!
end
~
#34607
пурж себя~
0 z 100
~
mpurge %self%
~
#34610
экзарх мертв - лоад сетшмотки~
0 f 100
~
if %world.curobjs(3304)% < 1 && %random.100% < 3
  mload obj 3304
end
~
$~
