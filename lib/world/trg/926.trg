#92600
огонь ладанки~
1 b 100
~
wait 20t
halt
if !%self.worn_by%
  wait 2t
  halt
else  
  set actor %self.worn_by%        
  osend %actor% Пламя вечного огня окружило вас!
  oechoaround %actor% Пламя вечного огня окружило %actor.vname%!
  dg_affect %actor% интеллект огненный_щит 3 150 1
  wait 8t
end
~
#92601
вскрыть замок~
1 c 4
вскрыть~
if !%arg.contains(замок)%
  osend %actor% Что вы хотите взломать???
  halt
elseif %actor.skill(взломать)% < 5
  osend %actor% Сначала научитесь взламывать а потом уже пытайтесь...
  halt
elseif %actor.vnum% > 0
  halt
end
switch %self.vnum%
  case 92630
    set sroom 92638
    set dir south 
    set dir2 north 
    set troom 92639
    set slozhn 79
  break
  case 92626
    set sroom 92638
    set dir north   
    set dir2 south 
    set troom 92640
    set slozhn 89
  break
  case 92627
    set sroom 92637
    set dir north
    set dir2 south  
    set troom 92641
    set slozhn 99
  break
  case 92628
    set sroom 92658
    set dir east
    set dir2 west
    set troom 92660
    set slozhn 79
  break
  case 92629
    set sroom 92659
    set dir west 
    set dir2 east  
    set troom 92661
    set slozhn 94
  break
  case 92631
    set sroom 92662
    set dir north
    set dir2 south  
    set troom 92663
    set slozhn 109
  break  
  default
    halt
  break                 
done                       
if %actor.skill(взломать)% > %slozhn%
  osend %actor% Хитрый механизм, пришла в голову мысль, но если...
  osend %actor% Правая рука рефлекторно потянулась за отмычкой.
  osend %actor% Вы ловко ковырнули отмычкой, и дверь с грохотом распахнулась!
  oechoaround %actor% %actor.name% осмотрел%actor.g% замок, в ^%actor.name% глазах мелькнула догадка!
  oechoaround %actor% %actor.name% привычным движением потянул%actor.u% за отмычкой.
  oechoaround %actor% %actor.name% начал%actor.g% ковырятся отмычкой в замке.
  oechoaround %actor% Замок щелкнул и дверь с грохотом распахнулась!    
  odoor %sroom% %dir% purge  
  odoor %sroom% %dir% room %troom%
  odoor %troom% %dir2% purge  
  odoor %troom% %dir2% room %sroom%
  wait 1s
  opurge %self%
else
  osend %actor% Вашего умения не хватит чтобы вскрыть этот замок, не тяните руки!
end
return 0
~
#92602
репоп взломанных дверей~
1 n 100
~
switch %self.vnum%
  case 92630
    set sroom 92638
    set dir south 
    set troom 92639
  break
  case 92626
    set sroom 92638
    set dir nurth   
    set troom 92640
  break
  case 92627
    set sroom 92637
    set dir north  
    set troom 92641
  break
  case 92628
    set sroom 92658
    set dir east
    set troom 92660
  break
  case 92629
    set sroom 92659
    set dir west   
    set troom 92661
  break
  case 92631
    set sroom 92662
    set dir north  
    set troom 92663
  break
  default
    halt
  break         
done                       
odoor %sroom% %dir% purge  
odoor %sroom% %dir% flags abcd
detach 92602 %self.id%
~
#92603
боевой тригг теневого дракона~
0 k 100
~
eval wai %random.3%*2+4
wait %wai%s
mtransform 92605
wait 2s
mtransform 92604
~
#92604
тригг чернока~
0 k 100
~
if %self.position% < 7 || %self.affect(молчание)%
  wait 1s
  halt
end
if !%world.curmobs(92604)% && !%world.curmobs(92605)% && %random.12% == 1
  mecho _%self.name% произнес%self.g% могущественное заклинание!
  mecho _Тени вокруг Вас задрожали и собрались в серое облако!
  wait 1s
  mecho _Серое облако взорвалось!
  mecho _Из вспышки взрыва появился огромный Теневой Дракон!
  mload mob 92604
  halt
end                            
set life 5
eval life (%self.hitp%/500)+%life%
if %life% < %random.10% && %random.5% == 1
  mecho _%self.name% выкрикну%seld.g% страшное заклинание!
  eval bonus %random.8%*1000+%self.maxhitp%
  %self.hitp(+%bonus%)%
  mecho _Силы смерти наполнили тело %self.rname%!
  wait 1s
  halt
end    
if %self.realroom% == 92667 && %self.hitp% < 1000 && %random.5% == 1
  mecho %self.name% взял свиток возврата в левую руку.
  mecho %self.name% зачитал свиток возврата.
  mteleport %self% 92662  
  dg_cast 'исцел'
end     
~
#92605
баттлетригг саламандры~
0 k 100
~
dg_cast 'обж хватк'
dg_cast 'обж хватк' 
dg_cast 'обж хватк'
~
#92606
табити мертва~
0 f 100
~
if %random.6% == 1 && %world.curobjs(92600)% < 2
  mload obj 92600
elseif %random.6% == 1 && %world.curobjs(92601)% < 2
  mload obj 92601
elseif %random.5% == 1 && %world.curobjs(92602)% < 2
  mload obj 92602
elseif %random.5% == 1 && %world.curobjs(92603)% < 2
  mload obj 92603
end
~
#92607
большая саламандра мертва~
0 f 100
~
if %random.6% == 1 && %world.curobjs(92607)% < 2
  mload obj 92607
elseif %random.5% == 1 && %world.curobjs(92608)% < 2
  mload obj 92608
end
~
#92608
великанша мертва~
0 f 100
~
if %random.6% == 1 && %world.curobjs(92604)% < 2
  mload obj 92604
elseif %random.6% == 1 && %world.curobjs(92605)% < 2
  mload obj 92605
elseif %random.5% == 1 && %world.curobjs(92606)% < 2
  mload obj 92606
elseif %random.4% == 1 && %world.curobjs(92609)% < 2
  mload obj 92609
end
~
#92609
вождь великанов  мертв~
0 f 100
~
if %random.6% == 1 && %world.curobjs(92610)% < 2
  mload obj 92610
elseif %random.6% == 1 && %world.curobjs(92611)% < 2
  mload obj 92611
elseif %random.5% == 1 && %world.curobjs(92612)% < 2
  mload obj 92612
elseif %random.5% == 1 && %world.curobjs(92613)% < 2
  mload obj 92613
end
~
#92610
слепой великан мертв~
0 f 100
~
if %random.6% == 1 && %world.curobjs(92614)% < 2
  mload obj 92614
elseif %random.6% == 1 && %world.curobjs(92615)% < 2
  mload obj 92615
elseif %random.5% == 1 && %world.curobjs(92616)% < 2
  mload obj 92616
elseif %random.5% == 1 && %world.curobjs(92617)% < 2
  mload obj 92617
end
~
#92611
хранитель огня умер 1~
0 f 100
~
if %random.7% == 1 && %world.curobjs(92621)% < 2
  mload obj 92621
end
~
#92612
хранитель огня умер 2~
0 f 100
~
if %random.7% == 1 && %world.curobjs(92622)% < 2
  mload obj 92622
end
~
#92613
умер охотник~
0 f 100
~
if %random.6% == 1 && %world.curobjs(92623)% < 2
  mload obj 92623
end
~
#92614
баттлетригг  финиста~
0 k 100
~
eval wai %random.4%*2+4
wait %wai%s
if %self.vnum% == 92620
  mtransform 92621
else
  mtransform 92620
end
~
#92615
финист умер~
0 f 100
~
if %random.6% == 1 && %world.curobjs(92624)% < 2
  mload obj 92624
elseif %random.5% == 1 && %world.curobjs(92625)% < 2
  mload obj 92625
end
~
#92616
боевой тригг огненного змея~
0 k 100
~
set i 0
if %self.hitp% < 10000
  eval i (%self.hitp%)/1000+4
  if !%self.affect(молчание)% && %self.position% > 6 && %random.18% < %i%
    mecho _Небесное пламя окутало Огненного змея!
    dg_cast 'защ бог'
    dg_cast 'исцел'
    halt
  end
end
eval wai %random.3%*2+4
wait %wai%s
mtransform 92622
wait 4s
mtransform 92623
~
#92617
вызов  чармистов  черноками в  мир~
0 b 100
~
if %self.fighting% || %random.pc%
  halt
end  
if %self.vnum% == 92601
  wait 3s
end
if %random.4% == 1
  eval rnum 92600+%random.36%
else
  eval rnum 92647+%random.9%
end  
eval mnum %random.3%+92627    
eval quanty (92631-%mnum%)*3
if %world.curmobs(%mnum%)% < %quanty%
  mat %rnum% mecho _%self.name% открыл врата вызова.
  mload mob %mnum%       
  mdoor %self.realroom% down room %rnum%
  mforce чармист_926 вниз                       
  mdoor %self.realroom% down purge
end
~
#92618
боевой  тригг  колдуна~
0 k 100
~
if %self.position% < 7 || %self.affect(молчание)%
  wait 2s
  halt
end           
set  vnum 0
eval i %random.3%+1
if %self.vnum% == 92608 && !%world.curmobs(92614)%
  set vnum 92614
elseif %self.vnum% == 92610 && !%world.curmobs(92615)%
  set vnum 92615
end
if %vnum%   
  mecho %self.name% раздвоился!
  while %i% >0
    mload mob %vnum%
    eval i %i%-1
  done
  halt
end             
if %self.realroom% != 92640 && %self.hitp% < 1000 && %random.5% == 1
  mecho %self.name% взял свиток возврата в левую руку.
  mecho %self.name% зачитал свиток возврата.
  mteleport %self% 92640 
  dg_cast 'исцел'
end
~
#92619
рейды мобов~
2 e 100
~
eval wt %random.25%+5
wait %wt%s 
if !%random.pc%
  halt
end
set actor %random.pc%
set i 0
foreach next %actor.pc%
  if %next.fighting%
    set i 1
  break
end
done
if !%i%
  halt
end
switch %random.7%
  *дух чернокнижника,  если живы сами чернокнижники
  case 1 
    if %world.curmobs(92601)% || %world.curmobs(92606)% 
      mecho _Серый портал открылся над Вами!
      mecho Дух чернокнижника прилетел сверху! 
      mload mob 92630
      wait 10t
    end
  break       
  *Ахмет,если живы ахмет и амал
  case 2
    case 3 
      if %world.curmobs(92609)% || %world.curmobs(92611)% 
        mecho Лазурная пентаграмма появилась в воздухе.
        mecho Ахмет появился из пентаграммы!
        mecho Пентаграмма медленно растаяла.
        mteleport ахмет_моб %self.vnum%
        wait 10t
      end
    break                    
    *Огненный воин если живы колдуны
    default
      if %world.curmobs(92608)% || %world.curmobs(92610)% 
        mecho Лазурная пентаграмма появилась в воздухе.
        mecho Огненный воин появился из пентаграммы!
        mecho Пентаграмма медленно растаяла.
        mload mob 92627
        wait 10t
      end
    break
  done
~
#92620
рекол  ахмета~
0 k 100
~
if %self.realroom% != 92677 && %self.hitp% < 1000 && %random.5% == 1
  mecho %self.name% взял свиток возврата в левую руку.
  mecho %self.name% зачитал свиток возврата.
  mteleport %self% 92677
  dg_cast 'исцел'
end
~
#92621
колдуны хелпят седому великану~
2 b 100
~
if !%random.pc% || !%world.curmobs(92617)%
  wait 4t
  halt
end
switch %random.3%
  case 1
    if !%world.curmobs(92608)%
      halt
    else
      calcuid kold 92608 mob
    end
  break
  case 2 
    if !%world.curmobs(92610)%
      halt
    else
      calcuid kold 92610 mob
    end
  break
  default     
    wait 1t
    halt
  break
done                        
wteleport %kold% %self.vnum%
wecho %kold.name% появился в клубах дыма!
~
#92622
черноки хелпят табити~
2 b 100
~
if !%random.pc% || !%world.curmobs(92600)%
  wait 4t
  halt
end
switch %random.3%
  case 1
    if !%world.curmobs(92601)%
      halt
    else
      calcuid kold 92601 mob
    end
  break
  case 2
    if !%world.curmobs(92606)%
      halt
    else
      calcuid kold 92606 mob
    end
  break
  default     
    wait 1t
    halt
  break
done                        
wteleport %kold% %self.vnum%
wecho %kold.name% открыл врата призыва!
wecho %kold.name% прилетел сверху.
~
#92623
огненный  змей  "умер"~
0 f 0
~
mload mob 92631
~
#92624
волх  дает перышко~
0 n 100
~
mecho Огненный змей ударился о пол и перекинулся в добра молодца!
wait 2s
say Приветствую вас герои!
say Хорошую забаву вы мне устроили! 
say Силу-удаль свою показали!
wait 2s
say Может и с моим поручением сможете справиться ?
wait 1s
say Залетал я как-то соколом в ирирй-рай.
say Раздобыть хотел молодильные яблоки.
say Увидел там красну девицу, Лелю - дочь Сварога небесного.
wait 1s
красн
wait 1s
say И влюбился в нее я безсмерти.
say Ради нее не стал покорять царство небесное.
say И она меня полюбила.
wait 1s
взд
wait 1s
say Но сестры ее, Жива с Мореной, зачаровали лазейку.
say По которой я в Ирий пролетал.
say Нет жизни мне без любимой моей.
wait 2s
say Если сможешь %actor.name%, отнеси Леле перышко финиста.
say Передай любимой моей весточку.
say Что не забыл я ее и люблю по прежнему.
wait 2s
mload obj 92634
дать перы %actor%
бро перы
say Нельзя смертным тут долго оставаться.
say Какими бы могучими они не были -  это место не для живых.
say Ступайте отсюда быстрее - иначе никогда не найдете выхода.
mdoor 92678 east room 92500
wait 40s
mdoor 92678 east purge
~
#92625
даем перышко леле~
1 i 100
~
if %victim.vnum% != 92324 || !%world.curmobs(92631)%
  halt
end
wait 1s
oecho Леля задумчиво посмотрела на перышко, и, не сказав ни слова ушла !
oteleport %victim% 91646
wait 10s
oecho Спустя некоторое время из глубины сада вышли Волх с Лелей!
oteleport %victim% 91643
oteleport волх_моб 91643
calcuid vict 92631 mob
attach 92626 %vict.id%
exec 92626 %vict.id%  
set quester %actor%
remote quester %vict.id%
wait 1
opurge %self%
~
#92626
Конец квеста~
0 z 100
~
wait 1s        
mecho Волх поклонился Ладе.
say Здравствуй Лада-матушка.
say Просил я руки твоей дочери у отца ее, Сварога небесного.
say И дал он свое согласие.
wait 2s 
say А ты, %quester.name%, честно заслужил%quester.g% награду!
if  %random.10% == 1 && !%actor.quested(926)%
  wait 2s
  say Я могу помочь тебе %quester%, достичь совершенства в любом твоем навыке.
  msend %quester% В каком умении ты хотел%quester.g% бы стать мастером ?
  attach 92627 %self.id% 
  detach 92626 %self.id%
elseif %random.10% == 1
  wait 1s
  say Возьми этот свиток, в нем записано заклинание огромной мощи, я думаю ты найдешь ему применение.
  switch %random.4%
    case 1
      mload obj 508
    break
    case 2
      mload obj 411
    break
    case 3
      mload obj 408
    break
    default
      mload obj 409
    break
  done
  дать все %quester.name%
  wait 1s
  detach 92626 %self.id%
end
set char %quester%
if %random.5% == 1  && %world.curobjs(92640)% < 2
  mload obj 92640
elseif %random.5% == 1  && %world.curobjs(92639)% < 2
  mload obj 92639
elseif %random.5% == 1  && %world.curobjs(92638)% < 2
  mload obj 92638
elseif %random.4% == 1  && %world.curobjs(92637)% < 2
  mload obj 92637
elseif %random.4% == 1  && %world.curobjs(92636)% < 2
  mload obj 92636
else
  mload obj 92635
end
дать все %quester.name%
detach 92626 %self.id%
~
#92627
награда скиллами~
0 c 1
прикрыть богатырский оглушить веерная точный осторожный метнуть заколоть сбить пнуть кулачный спасти украсть палицы секиры длинные короткие иное двуручники проникающее копья обезоружить луки дополнительный уклониться блокировать подножка ярость скрытый~
if %actor%==%quester%      
  if !%actor.skill(%cmd%)%             
    msend %actor% Как можно достичь совершенства в том, о чем даже не подозреваешь ?
    wait 5s
    halt
  elseif %actor.skill(%cmd%)% < 151
    mskilladd %actor.name% %cmd% 25 
    %actor.setquest(926)%
    detach 92627 %self.id%
  else
    eval skl %actor.skill(%cmd%)%
    eval val 200 - %skl%
    mskilladd %actor.name% %cmd% %val%
    %actor.setquest(926)%
    detach 92627 %self.id%
  end    
  
else
  say Я ценю твою помощь %quester.dname%, но одарить весь род людской не могу.
  wait 2s
  halt
end
~
$~
