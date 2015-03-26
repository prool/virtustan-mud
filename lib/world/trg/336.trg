#33600
у коваля~
0 b 50
~
wait 1
foreach fchar %self.pc%
  if !%fchar%
    halt
  end
  switch %random.15%
    case 1
      mecho Коваль ударил молотом и в стороны разлетелись каленые брызги.
      mecho Одна из брызг долетела до %fchar.rname% и обожгла.
      mdamage %fchar% 5
    break
    case 2
      mecho Коваль сильно ударил молотом и в стороны разлетелись каленые брызги.
      mecho Одна из брызг долетела до %fchar.rname% и обожгла.
      mdamage %fchare% 15
    break
    case 3
      say Работаем. Посторонись ка.
    break
    case 4
      mecho Коваль очень сильно ударил молотом и в стороны разлетелись каленые брызги.
      mecho Одна из брызг долетела до %fchar.rname% и обожгла.
      mdamage %fchar% 25
    break
    case 5
      mecho Коваль изо всех сил ударил молотом и в стороны разлетелись каленые брызги.
      mecho Одна из брызг долетела до %fchar.rname% и обожгла.
      mdamage %fchar% 55
    done
  done
~
#33601
у собаки~
0 q 100
~
wait 3s
mecho _Пес-дворняга старательно нюхает землю.
~
#33602
у девки~
0 q 100
~
wait 1
if      %actor.sex% == 1
  msend       %actor% Девица предложила Вам небольшой отдых.
  mechoaround %actor% Девушка сделала %actor.dname% весьма заманчивое предложение.
else
  msend       %actor% Девица взглянула на Вас и поняла, что толку ей с Вас, как с козла молока.
  mechoaround %actor% Девушка прошла мимо %actor.rname%, даже не взглянув не нее...
end
~
#33603
в мельнице~
2 b 100
~
wait 1
foreach fchar %self.pc%
  if %fchar.vnum% == -1
    switch %random.8%
      case 1
        wecho Жернова резко грохочут и у всех вокруг закладывает уши и болит голова.
        wdamage %fchar.name% 5
      break
      case 2
        wechoaround %fchar% Из грохочущих жернов вылетел камешек и попал %fchar.dname% в живот.
        wsend %fchar% Из грохочущих жернов вылетел камешек и попал Вам в живот.
        wdamage %fchar.name% 15
      break
      case 3
        wechoaround %fchar% Из грохочущих жернов вылетел камешек и попал %fchar.dname% в глаз.
        wsend %fchar% Из грохочущих жернов вылетел камешек и попал Вам в глаз.
        wdamage %fchar.name% 35
      break
      case 4
        wechoaround %fchar% Жернова развернулись и БОЛЬНО ударили %fchar.vname%.
        wsend %fchar% Жернова развернулись и БОЛЬНО ударили Вас.
        wdamage %fchar.name% 55
      break
      default
      break
    done
  end
done
~
#33604
помер лекарь~
0 f 100
~
*if (%world.curobjs(608)% < 40) && (%random.3% == 1)
*   mload obj 608
*end
~
#33605
подходят к холопу~
0 h 100
~
wait 1
if %actor.vnum% == 33833
  mshou Други! Татары у Киева! К оружию!
end
wait 1
if %actor.vnum% == 33834
  mshou  Помогите! Татарская орда наступает на Киев-град! За оружие!
end
~
$~
