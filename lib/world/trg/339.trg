#33900
гребешок~
0 f 10
~
if %world.curobjs(33901)% < 100
  mload obj 33901
end
if %world.curobjs(33912)% < 50
  mload obj 33912
end
~
#33901
куропатка померла~
0 f 10
~
if %world.curobjs(33916)% < 50
  mload obj 33916
end
~
#33902
кобыла померла~
0 f 10
~
if %world.curobjs(33915)% < 10
  mload obj 33915
end
~
#33903
корова померла~
0 f 10
~
if %world.curobjs(33904)% < 10
  mload obj 33904
end
~
#33904
крыса померла~
0 f 10
~
if %world.curobjs(33903)% < 5
  mload obj 33903
end
~
#33905
песпомер~
0 f 10
~
if %world.curobjs(33902)% < 6
  mload obj 33902
end
~
#33906
седойвоин_кто-топришел~
0 q 100
~
wait 1
msend %actor% Седой воин внимательно осмотрел Вас с головы до пят.
mechoaround %actor% Седой воин внимательно осмотрел %actor.vname% с головы до пят.
mecho Воин на мгновение задумался.
wait 1s
msend %actor% Потом вдруг снова взглянул на Вас.
mechoaround %actor% Потом вдруг снова взглянул на %actor.vname%.
wait 1s 
улыб %actor.name%
say Да, пожалуй ты сможешь мне помочь, %actor.name%...
wait 1s
say Готов%actor.g% ли ты мне помочь?
calcuid voin 33925 mob
attach 33907 %voin.id%
~
#33907
увоина~
0 d 0
да помогу~
wait 1
say Отлично!
wait 5
mechoaround %actor% Старый вояка почесал затылок, уселся поудобнее
mechoaround %actor% и начал что-то тихо говорить %actor.dname%.
msend %actor% Старый вояка почесал затылок и уселся поудобнее.
tell %actor.name% У нас в деревне происходят странные вещи.
tell %actor.name% Все началось после набега половцев.
tell %actor.name% Мой друг сражался как зверь!
tell %actor.name% Но, к сожалению, он потерял зрение в том бою...
взд
wait 10
tell %actor.name% И с тех пор с ним начало происходить что-то странное.
tell %actor.name% Однажды он сказал мне, что слышит голос, зовущий его с запада.
tell %actor.name% А на следующий день он исчез...
tell %actor.name% Я искал его, но не нашел.
wait 5
tell %actor.name% Возможно, ты отыщешь его, %actor.name%, он жил севернее колодца.
tell %actor.name% Там, в его доме, осталась его несчастная жена.
tell %actor.name% Она наверняка отблагодарит тебя, если ты найдешь ее мужа.
wait 1s
say Да, чуть не забыл! 
wait 5
msend %actor% Воин наклонился к Вам и начал шептать на ухо.
mechoaround %actor% Воин наклонился и что-то зашептал на ухо %actor.dname%
msend %actor% Он все говорил о каком-то заброшенном месте.
msend %actor% Но никто не знает где это...
wait 1s
mecho Воин опустил глаза и полностью погрузился в свои думы.
eval questor339 %actor.name%
worlds questor339
detach 33906 %self.id%
calcuid gena 33926 mob
attach 33909 %gena.id%
detach 33907 %self.id%
~
#33908
старушка ~
0 q 100
~
wait 1
msend %actor% Старушка осмотрела Вас с головы до пят.
mechoaround %actor% Старуха осмотрела %actor.rname% с головы до пят.
wait 1s
msend %actor% - Да не отвернут свой лик от тебя Боги, %actor.name%!
msend %actor% После этого она трижды Вас перекрестила и отвернулась.
mechoaround %actor% Старуха перекрестила %actor.vname% и отвернулась.
msend %actor% Мдя... Теперь Боги наверняка, Вас не покинут, а вражьи удары пропадут напрасно.
mechoaround %actor% Не троньте %actor.vname%! Он теперь под защитой Высших Сил!
~
#33909
утешитьжену~
0 c 0
утешить~
if (%arg.contains(женщин)% || %arg.contains(жена)% || %arg.contains(жену)%) 
  if %actor.name%==%questor339%
    wait 2
    calcuid gena 33926 mob
    msend %actor% Женщина взглянула на Вас мокрыми глазами, всхлипнула и перестала плакать.
    attach 33915 %self.id%
    run 33915 %self.id%
  else
    wait 2
    msend %actor% Женщина взглянула на Вас мокрыми глазами, всхлипнула
    msend %actor% и вновь разрыдалась.
  end
else
  return 0
end
~
#33910
удьякапросто~
0 q 100
~
wait 1s
msend %actor.name% Дьячок перестал писать, на мгновение задумался и продолжил дальше писать, не замечая Вас.
~
#33911
старухаовдове~
0 q 100
~
if (%actor.name%!=%questor339%)
  halt
end
wait 2
if %exist.mob(33927)%
  msend %actor.name% Старушка наклонилась к Вам и начала тихо говорить.
  tell %actor.name% %actor.name%, я молюсь день ночь за одну невинную женщину.
  mechoaround %actor% Старуха прижалась к %actor.dname% и стала еле слышно говорить.
  tell %actor.name% Вдова, живущая на окраине деревни, очень страдает!
  tell %actor.name% К ней является призрак ее мужа, и никто не может ей помочь.
  tell %actor.name% Помоги ей, и Боги тебя не оставят без внимания.
  mecho Старуха повернулась спиной к присутствующим и стала креститься.
  calcuid vdova 33910 mob
  attach 33916 %vdova.id%
end
detach 33911 %self.id%
~
#33912
дьяквыдачаквеста~
0 q 100
~
wait 1s
msend %actor% Дьячок перестал писать, на мгновение задумался и продолжил дальше писать, не замечая Вас.
if %actor.name%==%questor339%
  if %exist.mob(33927)%
    wait 1s
    msend %actor% Неожиданно дьячок поднял на Вас глаза и оглядел с головы до пят.
    msend %actor% - А, вояка. Тебя, наверно, прислала жена лучника. Ну что ж...
    wait 1s
    msend %actor% - На кладбище появляются духи - избавь нас от них, и тогда я, возможно, помогу тебе.
    msend %actor% Дьяк вновь занялся своими делами.
    calcuid nichiy 33915 mob
    attach 33913 %nichiy.id%
    attach 33930 %self.id%
    detach 33912 %self.id%
  end
end
~
#33913
убогийопризраке~
0 m 1
~
wait 1
if %amount% < 100
  say Спасибо государе, да продлятся Ваши дни на русской земле!
  halt
end
switch %myvar%
  case 1
    say На кладбище стали появляться духи!
    ужас
    wait 1
    mecho Убогий замолчал давая понять что знает еще что-то.
    set myvar 2
    global myvar
  break
  case 2
    say Только появляются они по ночам, поэтому днем на кладбище безопасно.
    wait 1
    mecho Убогий замолчал давая понять что знает еще что-то.
    set myvar 3
    global myvar
  break
  case 3
    say Еще люди гуторят, что там есть и источник зла! Большой призрак!
    wait 1
    mecho Убогий замолчал давая понять что знает еще что-то.
    set myvar 4
    global myvar
  break
  case 4
    say И что появляется он в полнолуние. Тогда туда лучше не показываться.
    wait 1
    mecho Убогий замолчал давая понять что знает еще что-то.
    set myvar 5
    global myvar
  break
  case 5
    say И еще говорят, будто призрак этот мужа женщины, живущей на окраине в старом доме...
    wait 1
    mecho Убогий замолчал, вдруг изловчился, прыгнул в кусты и пропал.
    set myvar 0
    global myvar
    mpurge %self.name%
  break
  default
    say Огромное спасибо, баре, да не отвернется от тебя удача!
    wait 1
    mecho Убогий замолчал давая понять что знает что-то очень важное.
    set myvar 1
    global myvar
  break
done
~
#33914
убогийпросто~
0 q 100
~
%actor.wait(2)%
wait 1
mecho Убогий упал ниц и протянул к Вам свои грязные руки.
msend %actor% Вы уставились на убогого человека.
wait 1s
msend %actor% Помогите люди добрые, я сам не местный, дом сгорел, воры украли все мои деньги.
msend %actor% Мне надо не так уж и много - монет 100, а лучше 200, чтоб добраться до Киева.
wait 2
msend %actor% Убогий посмотрел на Вас жалобно и заткнулся.
~
#33915
утешилжену2~
0 z 100
~
%actor.wait(3)%
wait 2
if %actor.sex% == 1
  msend %actor% Вы... хм... утешили жену охотника... лучше бы этого никто не видел... 
  wait 1s
  say %actor.name%, я вижу ты на многое способен. 
  say Мой муж искал исчезнувшее поселение. 
  say И в последнее время он зачастил к местному дьяку.
  say Хотя и он очень страннен.
  wait 1s
  say Его мало кто видел. Так вот, муж говорил, что дьяк знает как туда пройти.
  say Вернее, как туда пройти безопаснее.
  wait 1s
  msend %actor% Женщина обняла Вас и нежно поцеловала: "Найди моего мужа, и ты не пожалеешь!"
  mechoaround %actor% Женщина обняла %actor.vname% и нежно поцеловала.
else
  msend %actor% Ну что ж, женщине легче понять женщину... Вы утешили как могли.
  wait 1s
  say %actor.name%, ты сильная женщина, я бы никогда не смогла взяться за оружие.
  say Мне кажется, тебе будет под силу найти моего мужа.
  say Он часто говорил о каком-то месте, будто оно скрыто от посторонних.
  say В ответ я тебя щедро награжу, супруг - это единственное что у меня осталось.
  say Моего мужа часто видели у нашей церквушки, говорят, что дьяк знает как пройти туда.
  say Пожалуйста, верни мне моего ненаглядного.
  плак
end
calcuid star 33917 mob
attach 33911 %star.id%
detach 33908 %star.id%
calcuid diak 33916 mob
detach 33910 %diak.id%
attach 33912 %diak.id%
detach 33909 %self.id%
detach 33915 %self.id%
~
#33916
вдова выдача задания~
0 q 100
~
if (%actor.name%==%questor339%) && %exist.mob(33927)%
  wait 1
  просн
  встать
  wait 1s
  mecho Вдова тепло улыбнулась и... вдруг неожиданно разрыдалась.
  wait 1s
  mecho Успокоившись, снова, как бы виновато, улыбнулась.
  wait 1s
  msend %actor% - Извините меня, но у меня большое горе. Мой муж когда-то давно погиб
  msend %actor% в неравном бою. И он не был предан земле.
  wait 1s      
  всхл
  wait 1s
  msend %actor% - Теперь же - продолжила женщина, - он приходит ко мне по ночам.
  msend %actor% Я немного знакома с колдовством, но чувствую. что не смогу долго
  msend %actor% ему противостоять. Прошу тебя, %actor.name%, останови его!
  wait 1s
  msend %actor% - В качестве платы я научу тебя кое-чему. Прошу тебя, помоги!
  msend %actor% Женщина тяжело вздохнула и продолжила.
  wait 1s
  msend %actor.name% - Он появляется в районе кладбища, но я не знаю, как его выманить.
  msend %actor.name% Удачи тебе! - сказала она и отвернулась.
  attach 33918 %self.id%
  detach 33916 %self.id%
end
~
#33917
иконассанком~
2 c 0
прикоснуться~
wait 1
if !(%arg.contains(лик)%) 
  return 0
  halt
end
wsend  %actor% Икона задрожала, начала светиться, и... вдруг яркая вспышка ослепила Вас!
wsend  %actor% Спустя мгновение Вы снова смогли видеть и с удивлением обнаружили, что
wsend  %actor% светитесь необычным голубоватым сиянием!
wload mob 33928
calcuid slav 33928 mob
attach 33923 %slav.id%
exec 33923 %slav.id%
wait 1s
wpurge сварог
detach 33917 %self.id%
~
#33918
увдовы с заданием~
0 q 100
~
if (%actor.name%==%questor339%) 
  if %exist.mob(33927)%
    wait 2
    всхлип
    msend %actor% - Ах, %actor.name%, он все еще мучает меня!
  else
    wait 2
    ул
    msend %actor% - Ты покончил%actor.g% с ним?
    attach 33920 %self.id%
    detach 33918 %self.id%
  end
end
~
#33919
доложитьосмертидьяку~
0 c 0
доложить~
if (%actor.name%!=%questor339%) 
  return 0
  halt
end
say Ну что ж, я вижу ты справил%actor.u%! Теперь я, пожалуй, открою тебе один секрет.
mecho Дьяк на мгновение задумался.
wait 8
say Икона с изображением Сварога имеет способность накладывать на людей 
mecho Невидимую защиту. На того, кто к ней прикоснется, снизойдет защита Богов!
wait 1s
say А тот, кого ты ищешь, ушел на запад. Поторопись!
if %world.curobjs(33935)% < 1
  mload obj 33935
  say Возьми это - пригодится...
  wait 2
  дать ключ %actor.name%
end
wait 2
mecho Дьяк отвел взгляд от Вас и принялся за свою прежнюю работу.
calcuid aaa 33981 room
attach 33917 %aaa.id%
detach 33919 %self.id%
~
#33920
доложитьосмертивдове~
0 c 0
доложить~
if (%actor.name%!=%questor339%) 
  return 0
  halt
end
say Ну что ж, я вижу ты справил%actor.u%! Теперь я, пожалуй, отблагодарю тебя.
wait 1s
mecho Вдова начала рыться в сундуках.
wait 1s
mecho Вот! - сказала она достав из сундука небольшую книгу.
say Возьми эти мои сбережения и книгу в знак благодарности.
mload obj 33900
mload obj 33929
дать кол %actor.name% 
дать гор %actor.name%
wait 1s
mecho Вдова улыбнулась и помахала Вам на прощанье.
calcuid  vdova 33910 mob
detach 33920 %vdova.id%
~
#33921
удалениелучника~
2 z 100
~
wait 1s
%purge% questor3391
wecho Очень яркая вспышка ослепила Вас на мгновение! Да уж - присвистнули Вы...
if %world.curobjs(33934)% == 0
  wload obj 33934
end
~
#33922
загрузкаобъектовлучника~
0 n 100
~
if %world.curobjs(33905)% < 2
  mload obj 33905
end
if %world.curobjs(33913)% < 25
  mload obj 33913
end
if %world.curobjs(33914)% < 10
  mload obj 33914
end
if %world.curobjs(33925)% < 10
  mload obj 33925
end
~
#33923
сварог~
0 z 0
~
wait 1
mecho Сварог появился в клубах дыма!
say %actor.name%, иди, я незримо буду с тобой!
dg_cast 'san' %actor.name%
mecho Сварог исчез...
~
#33924
улучника~
0 r 100
~
wait 1s
появ
say О, спаситель мой! Спасибо тебе!
wait 1s
mecho Лучник задумался.
say Я искал потерянное поселение и обнаружил эти руины.
say Но вот беда, я где-то обронил свою счастливую стрелу.
say Там я слышал писк крыс. Найди ее и тогда я вернусь домой!
detach 33924 %self.id%
~
#33925
датьнаконечниклучнику~
0 j 100
~
wait 1
if %object.vnum% == 33934
  say Огромное спасибо! Это он, теперь я наконец обрету покой!
  mecho Лучник схватил наконечник и выкрикнув какое-то заклинание исчез в яркой вспышке.
  mecho Отнеси его моей жене! - донеслось уже откуда-то издалека.
  calcuid luks 33949 room
  exec 33921 %luks.id%
  calcuid genae 33926 mob
  attach 33927 %genae.id%
end
~
#33927
датьнаконечникжене~
0 j 100
~
wait 1
if !(%object.vnum% == 33934)
  msend %actor% Зачем мне это?
  eval getobject %object.name%
  броси %getobject.car%.%getobject.cdr%
  halt
end
if !(%actor.name%==%questor339%)
  msend %actor% Hе тебе давали задание, но за наконечник спасибо.
else
  wait 1
  mpurge %object%
  eval lev1 20-%actor.remort%/2
  msend %actor% Женщина взглянула на Вас и залилась горькими слезами.
  mechoaround %actor% Женщина взглянула на %actor.rname% и залилась горькими слезами.
  msend %actor% Мой муж - сквозь слезы проговорила она, - он мертв!
  плак
  wait 2
  say Ну чтож, теперь я хоть знаю его судьбу, видимо он сам этого захотел.
  say За это я тебя отблагодарю!
  switch %actor.class%
    *лекарь
    case 0
      if (!%actor.spelltype(изгнать зло)%) & (%actor.level%>=10)
        mspellturn %actor.name% изгнать.зло set
        say О, %actor.name%. Познай заклинание изгнать зло!
      else
        %self.gold(+1000)%
      end
    break
    *колдун
    case 1
      if (!%actor.spelltype(клонирование)% && ( %actor.level% >= %lev1% ))
        mspellturn %actor.name% клонирование set
        say О, %actor.name%. Пусть же ты сможешь размножаться без помощи других!
      end
    break
    *тать
    case 2
      if (!%actor.skill(подкрасться)%) & (%actor.level%>9)
        mskillturn %actor.name% подкрасться set
        say Вот знай теперь, как подкрадываться, %actor.name%.
      else
        say Извини, но либо ты мал%actor.g% еще, либо уже знаешь подкрасться. 
      end
    break
    *богатырь
    case 3
      if %actor.skill(ярость)% < 98
        if %actor.skill(ярость)% > 4
          mskilladd %actor.name% ярость %random.3%
        else
          mecho Извини, но я не могу тебя научить ничему новому...
        end
      else
        дум %actor.name%
        mecho Извини, я уже ничему новому тебя не научу...
        вздох
      end
    break
    *наемник
    case 4
      if %actor.skill(спрятаться)% < 98
        mskilladd %actor.name% спрятаться %random.3%
      else
        дум %actor.name%
        mecho Извини, я уже ничему новому тебя не научу...
        вздох
      end
    break
    *друж
    case 5
      if %actor.skill(пнуть)% < 98
        mskilladd %actor.name% пнуть %random.3%
      else
        дум %actor.name%
        mecho Извини, я уже ничему новому тебя не научу...
        вздох
      end
    break
    *кудесник
    case 6
      if (%actor.level%<18)
        say Ты слишком мал, чтоб я учил тебя заклинаниям, прими деньги.
        %actor.gold(+1000)%
      else
        if !%actor.spelltype(групповая сила)%
          mspellturn %actor.name% групп.сила set
          say О, %actor.name%. Теперь ты сможешь всех сотоварищей делать сильнее!
        else
          say Ты уже знаешь это заклинание, могу отблагодарить тебя только деньгами. 
          %actor.gold(+1000)%
        end
      break
      *Волшебники!
      case 7
        if (!%actor.spelltype(защитник)%) & (%actor.level%>=14)
          mspellturn %actor.name% защитник set
          say О, %actor.name%. Теперь ты сможешь призывать защитника!
        else
          say Извини, но либо ты мал%actor.g% еще, либо Хранитель может сопровождать тебя. 
        end
      break
      *чернок
      case 8
        if (!%actor.spelltype(увеличить жизнь)%) & (%actor.level%>=13)
          mspellturn %actor.name% увеличить.жизнь set
          say О, %actor.name%. Теперь ты будешь знать увеличение жизни!
        else
          say Извини, но либо ты мал%actor.g% еще, либо увеличение жизни тебе уже известно. 
        end
      break
      *витязь
      case 9
        if %actor.skill(обезоружить)% < 98
          mskilladd %actor.name% обезоружить %random.3%
        else
          дум %actor.name%
          mecho Извини, я уже ничему новому тебя не научу...
          вздох
        end
      break
      *охот
      case 10
        if %actor.skill(выследить)% < 98
          mskilladd %actor.name% выследить %random.3%
        else
          дум %actor.name%
          mecho Извини, я уже ничему новому тебя не научу...
          вздох
        end
      break
      *кузнец
      case 11
        if %actor.skill(оглушить)% < 98
          mskilladd %actor.name% оглушить %random.3%
        else
          дум %actor.name%
          mecho Извини, я уже ничему новому тебя не научу...
          вздох
        end
      break
      *купец
      case 12
        if (!%actor.spelltype(починка)%) & (%actor.level%>=25)
          mspellturn %actor.name% починка set
          say Вот знай теперь магическую починку, %actor.name%.
        else
          say Извини, но либо ты мал%actor.g% еще, либо уже знаком с магической починкой.
        end
      break
      *волхв
      case 13
        if ((%world.curobjs(206)% < 75) && (%random.1000% <= 100))
          mload obj 206
          say Получи награду... не очень полезная но....
          дать рун %actor.name%
        else
          %self.gold(+1000)%
          дать 1000 кун .%actor.name%
        end
      break
      default
        %self.gold(+1000)%
        дать 1000 кун %actor.name%
      break
    done
    wait 1
  end
  detach 33927 %self.id%
~
#33928
толстыйпомер~
0 f 100
~
if (%world.curobjs(528)% == 0) && (%random.1000% <= 150)
  mload obj 528
end
~
#33929
репоп квеста~
2 f 100
~
unset questor339
calcuid voin 33925 mob
attach 33906 %voin.id%
calcuid gena 33926 mob
detach 33909 %gena.id%
detach 33915 %gena.id%
detach 33927 %gena.id%
calcuid dyak 33916 mob
detach 33912 %dyak.id%
detach 33919 %dyak.id%
detach 33930 %dyak.id%
attach 33910 %dyak.id%
calcuid star 33917 mob
detach 33911 %star.id%
attach 33908 %star.id%
calcuid ubog 33915 mob
detach 33913 %ubog.id%
attach 33914 %ubog.id%
calcuid vdov 33910 mob
detach 33916 %vdov.id%
detach 33918 %vdov.id%
detach 33920 %vdov.id%
calcuid iconroom 33981 room
detach 33917 %iconroom.id%
calcuid luch 33923 mob
attach 33924 %luch.id%
~
#33930
удьяка с заданием~
0 q 100
~
if %actor.name%!=%questor339%
  wait 1s
  msend %actor.name% Дьячок перестал писать, на мгновение задумался и продолжил дальше писать, не замечая Вас.
  halt
end
if %exist.mob(33927)%
  wait 1s
  msend %actor% Дьяк оторвался от своих дел и что-то проворчал себе под нос, глядя на Вас.
  wait 1s
  msend %actor% Дьяк начал Вас толкать к выходу, давая понять, что больше ему сказать нечего.
  halt
end
wait 1s
msend %actor% Дьяк оторвался от своих дел и наконец заметил Вас.
wait 2
say А, %actor.name%! Хм.. Неужто справил%actor.u% с этой нечистью?
say Ну-ка, ну-ка, расскажи.
wait 2
msend %actor% Дьячок посмотрел на Вас с интересом.
attach 33910 %self.id%
attach 33919 %self.id%
detach 33930 %self.id%
~
#33931
Лоад буки снять оцепенение со знахаря~
0 f 100
~
if %random.100% <= 20
  if %world.curobjs(561)% < 2
    mload obj 561
  end
end
~
#33932
Смерть урода и лоад мешка~
0 f 100
~
if (%random.1000% <= 700)
  mload obj 33931
end
if ((%actor.vnum% == -1) && %actor.quested(27012)%)
  mload obj 11407
  %actor.unsetquest(27012)%
  %actor.setquest(27013)%
else
  set target %actor.leader%
  if %target.quested(27012)%
    mload obj 11407
    %target.unsetquest(27012)%
    %target.setquest(27013)%
  end
end
~
#33933
Приветствие у знахаря~
0 q 100
~
if %self.fighting%
  halt
end
if %actor.quested(33905)% && ! %actor.quested(33906)%
  wait 1s
  mecho Знахарь достал из кармана восковую куколку и намекающе покачал ее в руках.
  halt
end
if %actor.quested(33902)%
  halt
end
wait 1s
emot подслеповато прищурился, вглядываясь в ваше лицо
say Ну наконец ты пришел! Тебя ведь послал...
wait 1
emot еще раз пригляделся к вам, и вдруг досадливо поморщился.
wait 2
say Ох, извини, с годами я стал плохо видеть
say Впрочем... Не хочешь ли ты заработать?
attach 33934 %self.id%
~
#33934
Выдача квеста у знахаря~
0 d 1
да нет хочу согласен~
wait 1
if %speech% == нет
  ворч
  say Ну и ступай тогда своей дорогой.
  detach 33934 %self.id%
  halt
end
mat 33906 mload mob 33929
mat 33906 mload mob 33930
%actor.setquest(33902)%
say Вот и хорошо! 
mecho _- Нужен мне дельный человек, с оружием знакомый и не из пугливых.
mecho _- Принести одну вещь от купца Бдигоста, что нынче вечером на постоялом дворе у Силантия заночевать должен.
wait 2
say Принести тайно, да отдать вещь мне в руки. Только мне!
mecho _- Выполнишь - заплачу щедро! А чтоб ты не вздумал%actor.g% убежать...
emot подхватил со стола короткий ножик и ловко отхватил несколько волос из вашей прически.
wait 1
say Вздумаешь обмануть - наложу проклятье смертное! А теперь иди, нужно побыстрее обернуться.
wait 2s
ухм
mecho _"Вот так, голубушка, теперь-то ты моей будешь" - пробормотал знахарь в сторону.
detach 33934 %self.id%
~
#33935
Не пускаем квестера после взятия квеста у знахаря, если не говорил с толстым~
2 e 100
~
if (%direction% != north) || %actor.quested(33904)% || !%actor.quested(33902)%
  halt
end
If %exist.mob(33929)% 
  calcuid man1 33929 mob
  if (%man1.realroom% == 33906)
    wsend %actor% %man1.name% преградил вам дорогу.
    return 0
    halt
  end
end
if %exist.mob(33930)%
  calcuid man2 33930 mob
  if (%man2.realroom% == 33905)
    wsend %actor% %man1.name% преградил вам дорогу.
    return 0
    halt
  end
end
~
#33936
Молодцы разговаривают или агрят~
0 q 100
~
wait 1
if (%direction% == south) && %actor.haveobj(38105)% && !%actor.quested(33904)%
  крича Ага, попал%actor.u%! 
  крича Держи, хватай!
  mkill %actor%
  halt
end
if (((%direction% == west) || (%direction% == north)) && %actor.quested(33902)% && !%actor.quested(33904)%
  %actor.setquest(33903)%
  say Стой!
  say Торопиться после будешь, а сейчас с тобой поговорить хочет человек уважаемый.
  say Ступай-ка давай... Иди в самый богатый двор, не ошибешься!
  emot подтолкнул вас к дороге на север
  %actor.wait(2)%
end
~
#33937
Толстый крестьянин дает задание~
0 q 100
~
if %self.fighting%
  halt
end
if %actor.quested(33905)% && ! %actor.quested(33906)% && %actor.quested(33904)%
  wait 1s
  хмур
  say Хоть бей, хоть воруй, а сговоренное принеси!
  halt
end
if !%actor.quested(33903)% || %actor.quested(33904)%
  halt
end
%actor.setquest(33904)%
wait 1s
взд
say Ага, значит пригласили тебя ребятки? Садись и слушай!
emot прошелся взад-вперед по комнате
wait 2
say Вещичку, что знахарю нужна, принесешь мне!
mecho _- Ему, стручку дряхлому, она лишняя будет.
mecho _- Что ты там с купцом сделаешь - твое дело, но если принесешь свиток мне
mecho _- То я... ты ведь из тех, что всякие там знания собирают?
mecho _- Так будет тебе награда, куда как немалая!
wait 2s
emot отвернулся от Вас и хмыкнул. 
mecho Толстый крестьянин пробормотал: "Сколько ж хлопот из-за бабы, да еще и вдовы!"
wait 1s
say Все, ступай.
calcuid batrak1 33929 mob
calcuid batrak2 33930 mob
if %batrak1%
  mpurge %batrak1%
end
if %batrak2%
  mpurge %batrak2%
end
~
#33938
Вход к вдове с заданиями знахаря и толстого~
0 q 100
~
wait 1
if !%actor.quested(33902)% || !%actor.quested(33904)% || %actor.quested(33907)%
  halt
end
wait 3
if %actor.sex% == 1
  say Здравствуй, человек прохожий.
else
  say Здрава будь!
end
Что привело тебя ко мне?
attach 33939 %self.id%
~
#33939
Вдова дает задание~
0 d 1
знахарь крестьянин богатей~
wait 1
%actor.setquest(33907)%
Ах, этот! 
emot тяжело вздохнула
wait 2s
say Трое их было, Знахарь, Богатей, да муж мой покойный.
mecho _- Всегда-то они во всем друг дружку обойти пытались.
mecho  - В богатстве, в славе, в умении. Завидовали, злословили.
wait 2
say А как умер муженек, так эти двое стали меня домогаться
mecho _- Ведь и хозяйство большое и жена пригожая почитай сами в руки просятся.
mecho _- Не бывать по ихнему! Знаю, знахарь с купцом заезжим сговаривался
mecho _- И соседушка все своих батраков на дорогу посылает...
wait 2s
say Мое слово крепко - коль вещь эту мне принесешь, то награжу вдвое против ихнего!
detach 33939 %self.id%
~
#33940
Сдача квеста знахарю~
0 j 100
~
wait 1
if %object.vnum% != 38105 
  дум
  drop %object.name%
  halt
end
wait 1
mpurge %object%
emot развернул свиток и прочитал написанное.
wait 2s
if !%actor.quested(33902)%
  say Хм, я тебя о помощи не просил, но это именно та вещь, что мне нужна.
  say Благодарствую.
  halt
end
%actor.unsetquest(33900)%
%actor.unsetquest(33901)%
%actor.unsetquest(33902)%
%actor.unsetquest(33903)%
%actor.unsetquest(33904)%
%actor.unsetquest(33905)%
%actor.unsetquest(33906)%
%actor.unsetquest(33907)%
%actor.setquest(33909)%
mat 33923 mload mob 33929
mat 33923 mload mob 33930
calcuid batrak2 33930 mob
detach 33936 %batrak2.id%
calcuid batrak 33930 mob
attach 33943 %batrak.id%
say Да, это он. Вот, получи награду обещанную.. 
if ((%actor.class% == 0) && !%actor.spelltype(критическое исцеление)%)
  wait 1
  say так и быть, за помощь научу тебя лекарской премудрости одной.
  msend %actor% Знахарь дал вам несколько наставлений.
  mechoaround %actor% Знахарь склонился с %actor.dname% и ему прошептал.
else
  mload obj 562
  дать книг .%actor.name%
end
wait 1
say И совет мой - иди-ка ты отсюда побыстрей, человек прохожий!
detach 33933 %self.id%
detach 33934 %self.id%
~
#33941
Выдача награды у толстого крестьянина~
0 j 100
~
wait 1
if %object.vnum% != 38105 
  дум
  drop %object.name%
  halt
end
wait 1
mpurge %object%
emot развернул свиток и прочитал написанное
wait 2s
if !%actor.quested(33904)%
  say Хм, я тебя о помощи не просил, но это именно та вещь, что мне нужна.
  say Благодарствую.
  halt
end
%actor.unsetquest(33900)%
%actor.unsetquest(33901)%
%actor.unsetquest(33902)%
%actor.unsetquest(33903)%
%actor.unsetquest(33904)%
%actor.unsetquest(33905)%
%actor.unsetquest(33906)%
%actor.unsetquest(33907)%
say Вот оно что. Хитер знахарь, вот чем взять решил!
mecho _- Чтож, дело сделано, пора плату платить
switch %random.3%
  case 1
    case 2
      case 3
        mload obj 33940
        say У меня дед был ба-альшой знаток колдовства всякого. 
        mecho _- Свиток от него остался. Буковки повытерлись, ну так разобрать можно.
        mecho _- Думаю, тебе сгодится.
        give свиток .%actor.name%
      break
      case 4
        case 5
          mload obj 528
          say У меня дед был ба-альшой знаток колдовства всякого.
          mecho _- Осталось у меня кой-чего из его хозяйства. Вот, держи.
          give книг .%actor.name%
        break
        default
          %self.gold(+500)%
          give 500 кун .%actor.name%
        done
        set victim %actor%
        global victim
        calcuid medic 33914 mob
        remote victim %medic.id%
        rdelete victim %self.id%
        wait 1s
        exec 33945 %medic.id%
~
#33942
Выдача награды у вдовы~
0 j 100
~
wait 1
if %object.vnum% != 38105 
  дум
  drop %object.name%
  halt
end
wait 1
mpurge %object%
emot развернула свиток и прочитала написанное
wait 2s
if !%actor.quested(33902)%
  say Хм, я тебя о помощи не просила, но это именно та вещь, что мне нужна.
  say Благодарствую.
  halt
end
%actor.unsetquest(33900)%
%actor.unsetquest(33901)%
%actor.unsetquest(33902)%
%actor.unsetquest(33903)%
%actor.unsetquest(33904)%
%actor.unsetquest(33905)%
%actor.unsetquest(33906)%
%actor.unsetquest(33907)%
%actor.setquest(33909)%
mat 33920 mload mob 33929
mat 33920 mload mob 33930
calcuid batrak2 33930 mob
detach 33936 %batrak2.id%
calcuid batrak 33930 mob
attach 33943 %batrak.id%
mecho Вдова скомкала тонкую кожу и бросила в печной огонь.
say Спаси тебя боги, %actor.name%! Уберег%actor.q% ты меня...
wait 2
say Ну а раз обещала, то вот и награда.
say Муж мой по миру немало пошлялся, да всякого домой привез.
switch %random.7%
  case 1
    case 2
      case 3
        mload obj 561
        give книг .%actor.name%
      break
      case 4
        mload obj 245
        дать рун .%actor.name%
      break
      default
        %self.gold(+500)%
        give 500 кун .%actor.name%
      done
      if ((%actor.align% > -200) && (%actor.align% < 100))
        %actor.align(+5)%
      end
      set victim %actor%
      global victim
      calcuid medic 33914 mob
      remote victim %medic.id%
      rdelete victim %self.id%
      wait 1s
      exec 33945 %medic.id%
~
#33943
Батраки агрят~
0 q 100
~
wait 1
if !%actor.quested(33909)%
  halt
end
if ((%direction% == west ) || (%direction% == north ))
  крича Ага! Не уйдешь!
  mkill %actor%
  mforce batrak339_1 kill .%actor.name%
end
~
#33944
Убираем проклятие с игрока если знахарь убит~
0 f 100
~
foreach target %self.pc%
  if %target.quested(33908)%
    %target.unsetquest(33908)%
    dg_affect %target% инициатива плач -20 0
    dg_affect %target% ловкость плач -5 0
    dg_affect %target% сила плач -5 0
    dg_affect %target% интеллект проклятье -5 0
    dg_affect %target% мудрость проклятье -5 0
    dg_affect %target% телосложение проклятье -5 0
    dg_affect %target% обаяние проклятье -5 0
    dg_affect %target% удача медлительность -50 0
    dg_affect %target% запоминание медлительность -100 0
    dg_affect %target% успех.колдовства медлительность -50 0
    halt
  end
done
foreach target %self.pc%
  if %target.quested(33902)% && !%target.quested(33908)%
    %target.setquest(33908)%
    say Так этот мерзавец подкупил тебя!
    msend %target% "Так будь же ты проклят!" - взвизгнул знахарь, вперив в вас палец.
    mechoaround %target% "Так будь же ты проклят!" - взвизгнул знахарь, вперив в %target.vname% палец.
    msend %victim% Все вокруг внезапно померкло и страшная боль пронзила ваше тело.
    dg_affect %victim% инициатива плач -20 10000 2
    dg_affect %victim% ловкость плач -5 10000 2
    dg_affect %victim% сила плач -5 10000 2
    dg_affect %victim% интеллект проклятье -5 10000 2
    dg_affect %victim% мудрость проклятье -5 10000 2
    dg_affect %victim% телосложение проклятье -5 10000 2
    dg_affect %victim% обаяние проклятье -5 10000 2
    dg_affect %victim% удача медлительность -50 10000 2
    dg_affect %victim% запоминание медлительность -100 10000 2
    dg_affect %victim% успех.колдовства медлительность -50 10000 2
    halt
  end
done
~
#33945
Знахарь проклинает~
0 z 100
~
msend %victim% _Вдруг откуда-то вы услышали шепот знахаря:
msend %victim% ___"Я же говорил, чтобы ты не пытал%victim.u% меня провести!
msend %victim% ____Получай теперь, что заслужил%victim.g%!"
msend %victim% Все вокруг внезапно померкло и страшная боль пронзила ваше тело.
dg_affect %victim% инициатива плач -20 0
dg_affect %victim% ловкость плач -5 0
dg_affect %victim% сила плач -5 0
dg_affect %victim% интеллект проклятье -5 0
dg_affect %victim% мудрость проклятье -5 0
dg_affect %victim% телосложение проклятье -5 0
dg_affect %victim% обаяние проклятье -5 0
dg_affect %victim% удача медлительность -50 0
dg_affect %victim% запоминание медлительность -100 0
dg_affect %victim% успех.колдовства медлительность -50 0
dg_affect %victim% инициатива плач -20 10000 2
dg_affect %victim% ловкость плач -5 10000 2
dg_affect %victim% сила плач -5 10000 2
dg_affect %victim% интеллект проклятье -5 10000 2
dg_affect %victim% мудрость проклятье -5 10000 2
dg_affect %victim% телосложение проклятье -5 10000 2
dg_affect %victim% обаяние проклятье -5 10000 2
dg_affect %victim% удача медлительность -50 10000 2
dg_affect %victim% запоминание медлительность -100 10000 2
dg_affect %victim% успех.колдовства медлительность -50 10000 2
%victim.setquest(33908)%
~
#33999
Метки квестов в зоне 339~
0 g 100
~
33900 - базовая метка, висит пока делается квест
33901 - метка о выполнении квеста
33902 - игрок брал задание у знахаря
33903 - игрок заходил к молодцам
33904 - игрок получил задание от крестьянина
33905 - игрок заходил к купцу
33906 - игрок получил свиток от купца
33907 - получил задание от вдовы
33908 - игрок проклят знахарем
~
$~
