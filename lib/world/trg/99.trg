#9900
����� � ����-��������~
0 r 100
~
If %exist.obj(9986)% == 1
  wait 1s
  say ��-��-��, ����� ������ �����. ���� � ���� � ������ ���� �����, � ��� �������.
  wait 1s
  say � ����� ���-�� ����� �� ��� � ������� ���� ����: �� �� �� �����, �� �� � �������.
  wait 1s
  say ������ ���! ����� ��� ����, � � � ����� �� ��������.
else
  Halt
end
~
#9901
����� ������� ����-��������~
0 j 9986
100~
wait 1s
if %object.vnum% == 9986
  say �������������, �� ���� �������%actor.g%. ������ � ���� ���! 
  switch %random.3%
    case 1
      say ��� ���� ������� �� ���.
      mload obj 9946
      give ������ .%actor.name%
    break
    case 2
      say � ������� � ��� ���� 150 �����, ���� ����� � ����� ��������
      %self.gold(+150)%
      give 150 ���� %actor.name%
    break
    case 3
      say � ���������, ��� ����� ���� ���������.
    break
  done
  mpurge ����
else
  say ���?
  wait 2s
  say ��� ���?! ������ �� ���� ��� �����!
  wait 1s
  eval getobject %object.name%
  if %getobject.car% == ����
    mpurge ����
  else
    ���� %getobject.car%.%getobject.cdr%
  end
end
~
#9902
������ ���������~
0 d 100
������~
say ���� ��, ������, ����������� �� ������� ������� ���� �����������
say (� �����) � ��������� ��� ������ ������� �����, � ���� �������.
say ��-��-��! ��� ��!
done
~
#9903
������~
0 a 100
~

~
#9950
�������� � �������~
2 c 100
������~
wait 1
%portal% 7028 1
~
#9951
�������� �� ������~
2 c 100
������~
wait 1
%portal% 21038 1
~
#9989
�����������~
0 d 100
��������~
� ������, �� ������ � �� ���� ���� ��������������� ���� ����
~
#9990
����������� �������~
0 h 100
~
wait 1s
� ������, %actor.name%
wait 1s
� ���� �� ������ ����-������ �������������, �� ����� ����� ��������.
wait 1s
� ���� ���� ����� �������� ����������, �� ��� � �����
wait 1s
��� .%actor.name%
~
#9991
������ ������~
0 d 100
��������~
if (!%arg%.contains(����))
  � ��, � �� ���������, ������� ���
  halt
end
� ���, %actor.name% ��� ������� � ������ ���� !
wait 1s
emo �������� ������� �����
dg_cast '����' %actor.name%
dg_cast '�����' %actor.name%
dg_cast '��� ���' %actor.name%
dg_cast '����' %actor.name%
dg_cast '�����' %actor.name%
dg_cast '�����' %actor.name%
dg_cast '�����' %actor.name%
dg_cast '����' %actor.name%
dg_cast '����' %actor.name%
dg_cast '��� ���' %actor.name%
dg_cast '��� ���' %actor.name%
dg_cast '���� ��' %actor.name%
dg_cast '���� ��' %actor.name%
dg_cast '��� ���' %actor.name%
wait 1s
���
� �� ��� � �ӣ
��� %actor.name%
~
#9992
���� � �����~
1 bz 100
~
done
wait 1
switch %random.12%
  Case 1
    oecho �� ������� ����� ����������� �������: "���������"
  break
  case 2
    oecho ����� ���� ������� ����������� ������: "� ������ �� �������� ���������!"
  break
  case 3
    oecho "� �����", - �������������� ������� ��� ���� ���� �������,
    oecho ����� ����� �������� ������� ����������
  break
  case 4
    oecho "�� �� ��������, �� �� ��������...", - ������ ������, �������� ���������
    case 5
      oecho ���� �������, � ����������� �����, ������ �������, ������� ����,
      oecho ���� � �������, � ������������.
    break
  done
~
#9993
new trigger~
0 c 100
����~
mecho ����� � ����� ����� ��� �� ������!
dg_cast '��� �����'
done
~
#9994
prool trig 1~
2 d 100
���~
say My trigger commandlist is not complete!
say ��, ���!
~
$~
