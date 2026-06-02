.386
.MODEL FLAT
OPTION CASEMAP: NONE
EXTERN  _lstrlenA@4: PROC; функция определения длины строки

PUBLIC _string_compression@8; директива для объявления данных общедоступными

.CODE; сегмент кода 

; процедура сжатия строки
_string_compression@8 PROC
PUSH EBP; сохраняем регистр базы
MOV EBP, ESP; устанавливаем регистр базы
; сохраняем регистры
PUSH ESI; источник
PUSH EDI; приемник  
PUSH EBX; 
PUSH ECX; счетчик
PUSH EDX; длина результата
    
; получаем данные из стека
MOV ESI, [EBP+8]; адрес исходной строки
MOV EDI, [EBP+12]; адрес результата
    
; определяем длину исходной строки
PUSH ESI
CALL _lstrlenA@4
MOV ECX, EAX   
CMP ECX, 0
JE end_processing; если строка пустая, переходим к концу
    
CLD; очищаем флаг направления DF (для движения слева направо)
XOR EDX, EDX; обнуляем счетчик длины результата
   
symbol_processing:
CMP ECX, 0
JE end_processing; если строка пустая, переходим к концу
LODS BYTE PTR [ESI]; загружаем элемент цепочки в регистр AL
PUSH EDI; сохраняем текущую позицию в результате
PUSH ECX; сохраняем счетчик основного цикла
MOV EBX, [EBP+12]; начало результата для поиска
MOV ECX, EDX; текущая длина результата
CMP ECX, 0
JE add_symbol; если результат пустой, добавляем символ
    
check:
CMP AL, [EBX]; сравниваем символ с текущим в результате
JE skip_symbol; если символ найден, пропускаем добавление
INC EBX; переходим к следующему символу в результате
LOOP check
    
add_symbol:
POP ECX; восстанавливаем счетчик цикла
POP EDI; восстанавливаем позицию в результате
STOS BYTE PTR [EDI]; сохраняем байт из AL в результат
INC EDX; увеличиваем длину результата
JMP next_symbol; переходим к следующему символу
    
skip_symbol:
POP ECX; восстанавливаем счетчик цикла
POP EDI; восстанавливаем позицию в результате
    
next_symbol:
DEC ECX; уменьшаем счетчик оставшихся символов
JMP symbol_processing; возвращаемся в начало цикла
    
end_processing:
MOV BYTE PTR [EDI], 0; завершаем строку нулем
; восстанавливаем регистры
POP EDX
POP ECX
POP EBX
POP EDI
POP ESI
POP EBP
RET 8; очищаем стек от 2 параметров
_string_compression@8 ENDP

END
