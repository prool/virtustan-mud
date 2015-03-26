#68800
шпион говорит с игроком~
0 r 100
~
wait 5
msend %actor%  Шпион внимательно посмотрел на вас.
msend %actor%  - Эй, ты!
msend %actor%  - А ну-ка, погодь.
msend %actor% _- У меня к тебе дело есть...
msend %actor% _- Хорошо оплачиваемое.
msend %actor% _- Ты человек здесь новый, неприметный.
msend %actor% _- Тебе будет легче справиться с этой работенкой.
wait 10
msend %actor% _- Ну как, желаешь подзаработать?
~
#68801
шпион выдает квест~
0 d 0
да помогу хорошо продолжай~
wait 10          
detach 68800 %self.id%
set quester %actor%
global quester
msend %actor%  Шпион ухмыльнулся.
msend %actor% _- Я знал, что ты согласишься.
msend %actor% _- Думаю, лишние деньги тебе бы пригодились.
wait 10
msend %actor% _- Так вот слушай, что тебе предстоит сделать.
msend %actor% _- В крепости есть воевода.
msend %actor% _- У него есть бронзовый колт, который нужен моему хозяину.
msend %actor%  - Если ты принесешь мне его, я щедро вознагражу тебя.
wait 10
msend %actor% _- Думаю, это все, что тебе стоит знать.
msend %actor% _- Иди же.  
detach 68801 %self.id%
~
#68802
шпион принимает колт~
0 j 100
~
wait 1
if %actor% != %quester%
  msend %actor%  - Слушай!
  msend %actor%  - Я не помню тебя.
  msend %actor%  - Зачем ты суешь мну эту дрянь?
  msend %actor%  - Подставить меня хочешь?!
  брос %object.name%
  halt
end
if %object.vnum% != 68807
  msend %actor%  - На кой нужна мне эта дрянь?
  msend %actor%  - Куда я, по-твоему, должен ее деть?!
  брос %object.name%
  halt
end
wait 1 
mpurge %object%
msend %actor%  Шпион внимательно посмотрел на вас.
say Вот молодчина!
mecho _- Не ожидал я от тебя такой прыти.
mecho _- Вот тебе твоя награда, за такой нелегкий труд.
switch %random.5%
  case 1
    if %world.curobjs(68808)% < 10 
      msend %actor%  - Вот тебе монетка, которую я всегда ношу с собой.
      msend %actor%  - С помощью ее всегда можно разрешить спор, не прибегая к насилию.
      msend %actor%  - Пусть она принесет тебе удачу.
      mload obj 68808    
      дать монетк %actor.name%
    end
  break
  case 2
    if %world.curobjs(68809)% < 10
      msend %actor%  - Вот, держи-ка этот кинжал.
      msend %actor%  - С его помощью можно убить противника,
      msend %actor%  - не издав при этом ни малейшего шума.
      mload obj 68809    
      дать кинж %actor.name%
    end
    case 3
      if %world.curobjs(68810)% < 10
        msend %actor%  - Вот тебе мой пояс.
        msend %actor%  - Его шипы помогут тебе в битве и нанесут дополнительные раны твоему врагу.
        mload obj 68810    
        дать пояс %actor.name%
      end
    break
    default
      msend %actor%  - Вот, держи этот мешочек кун.
      msend %actor%  Шпион дал Вам небольшую горсть кун.
      msend %actor%  Это составило 500 кун.
      %actor.gold(+500)%
      mechoaround %actor%  Шпион дал небольшую кучку кун %actor.dname%.
    done
~
#68803
поскрыпывают ворота~
2 e 25
~
wait 1
wecho _Ворота жалобно заскрипели и немного покосились.
~
#68804
бросили монетку~
1 h 100
~
wait 10
oechoaround %actor%  %actor.iname% подбросил%actor.g% монетку, и она звонко упала на землю.
osend %actor% _Вы подбросили монетку и она звонко упала на землю.
switch %random.2%
  case 1
    oecho _На монетке выпала сторона с изображением всадника.
  break
  case 2
    oecho _На монетке выпала сторона с изображением пятиглавого огнедышащего змея.
  break
done
~
#68805
РЕПОП~
2 f 100
~
calcuid fromob 68824 mob
attach 68800 %fromob.id%
attach 68801 %fromob.id%
calcuid fromob 68818 mob
attach 68806 %fromob.id%
attach 68807 %fromob.id%
calcuid fromob 68803 mob
attach 68809 %fromob.id%
attach 68810 %fromob.id%
~
#68806
кладовщик приветствует~
0 r 100
~
wait 10
msend %actor%  Кладовщик повернулся к вам.
if %actor.sex% == 2
  msend %actor%  - Привет, путешественница.
  msend %actor%  - Не желаешь ли заработать немного кун?
  msend %actor%  - Работенка не пыльная и очень легкая?
  msend %actor%  - Ну как, ты согласна?
else
  msend %actor%  - Привет, путешественник.
  msend %actor%  - Не желаешь ли заработать немного кун?
  msend %actor%  - Работенка не пыльная и очень легкая?
  msend %actor%  - Ну как, ты согласен?
end
~
#68807
кладовщик дает задание~
0 d 0
да помогу хорошо~
wait 1
detach 68806 %self.id%
msend %actor%  Кладовщик улыбнулся.
msend %actor% _- Вот и отлично.
msend %actor% _- Вот тебе мешочек.  
mload obj 68817
дать меш %actor.iname%
msend %actor% _- Отнеси его отроку в дом воеводы.
msend %actor% _- Он же тебе и заплатит за работу.
msend %actor% _- И поторопись!
detach 68807 %self.id%
~
#68808
отрок принимает мешочек и дает ключик~
0 j 100
~
if %object.iname% == маленький мешочек
  wait 1
  mpurge мешочек
  msend %actor%  Шустрый отрок кивнул.
  msend %actor% _- Отлично!
  msend %actor% _- Как раз вовремя.
  msend %actor%  - Я уж начал думать, что этот старый пень никого не пошлет сюда.
  msend %actor% _- Спасибо тебе за помощь.
  wait 6
  msend %actor% _Отрок покопался в темном углу и дал вам стальной ключик.
  msend %actor% _- На смотровой башне есть сундук.
  msend %actor% _- Все его содержимое - теперь твое.
  mechoaround %actor%  Отрок покопался в темном углу и дал %actor.dname% стальной ключик.
  mload obj 68813
  дать все .%actor.iname%
else  
  wait 5
  msend %actor% _- Хм. Это мне не нужно.
  msend %actor% _- Поэтому оставь это себе!
  дать %object.iname% .%actor.iname%
end
~
#68809
воевода приветствует~
0 r 100
~
wait 10
msend %actor%  Воевода повернулся к вам.
if %actor.sex% == 2
  msend %actor%  - Привет тебе, странница.
else
  msend %actor%  - Привет тебе, странник.
end
msend %actor%  - Рад видеть здесь хороших людей.
msend %actor%  - Вижу, ты очень устал%actor.g% с дороги.
msend %actor%  - Но все же, не откажи мне в небольшой услуге.
msend %actor%  - Поможешь ли ты старому человеку?
~
#68810
воевода дает задание~
0 d 0
да помогу хорошо продолжай~
wait 5 
detach 68809 %self.id%
msend %actor%  Воевода квинул.
msend %actor% _- Вот и ладушки.
msend %actor%  - Недавно, совершая обход по лагерю, я потерял свой браслет.
msend %actor% _- Я долго узнавал у воинов, куда он мог деться.
msend %actor% _- Но все только разводят руками.
wait 10
msend %actor%  - Подозреваю, что его кто-то куда-то спрятал и решил оставить себе.
msend %actor% _- Если ты найдешь его, я не постою за наградой.
detach 68810 %self.id%
~
#68811
воеводе дали браслет~
0 j 100
~
if %object.vnum% != 68818
  msend %actor%  - Это не моя вещь!
  msend %actor%  - Она мне не нужна.
  брос %object.iname%
  halt
end
wait 1
mpurge %object%
msend %actor%  Воевода обрадовался.
msend %actor%  - Ты все же смог%actor.g% найти его!
msend %actor%  - А я уже и не надеялся.
msend %actor%  - Я не желаю знать, кто уволок его, но ты заслужил%actor.g% награду. 
wait 5
if !%exist.obj(68807)%
  msend %actor% Воевода дал вам пригоршню монет.
  msend %actor% ...Но увы - все до одной они оказались неясно чьей чеканки :(
else
  сн колт
  дать колт %actor.iname%
  msend %actor%  - Вот, думаю, это щедрая награда.
  msend %actor%  - Пусть он защитит тебя.
end
~
$~
