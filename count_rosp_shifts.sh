#!/bin/bash

# Скрипт для подсчета роспусков по сменам
# Смены: дневная 08:00-20:00, ночная 20:00-08:00
# Учитывает периоды активности системы по логам (val=1 - активна, val=0 - неактивна)

# Параметры подключения к MySQL
DB_HOST="192.168.0.28"
DB_USER="kgm"
DB_PASS="gannimed"
DB_NAME="kgm_new"

# Путь к директории с логами
SOURCE_LOG_DIR="/kgm/log"  # Исходная директория с логами
LOG_DIR="/tmp"              # Временная директория для обработки логов

# Период анализа
START_DATE="2025-10-01"  # Дата начала в формате YYYY-MM-DD
END_DATE="2025-10-31"    # Дата окончания в формате YYYY-MM-DD

# Конвертируем даты в unix timestamp
# Начало первой дневной смены (08:00:00)
start_timestamp=$(date -d "$START_DATE 08:00:00" +%s)
# Конец периода
end_timestamp=$(date -d "$END_DATE 23:59:59" +%s)

# Длительность смены в секундах (12 часов)
SHIFT_DURATION=43200

# Функция для склонения слова "роспуск"
get_rosp_word() {
    local num=$1
    local last_digit=$((num % 10))
    local last_two_digits=$((num % 100))
    
    if [ $last_two_digits -ge 11 ] && [ $last_two_digits -le 14 ]; then
        echo "роспусков"
    elif [ $last_digit -eq 1 ]; then
        echo "роспуск"
    elif [ $last_digit -ge 2 ] && [ $last_digit -le 4 ]; then
        echo "роспуска"
    else
        echo "роспусков"
    fi
}

echo "Анализ роспусков по сменам с учетом активности системы"
echo "======================================================="
echo ""

# Шаг 1: Копирование и распаковка логов
echo "Шаг 1: Подготовка логов..."
echo "Копирование логов из $SOURCE_LOG_DIR в $LOG_DIR..."

# Создаем список файлов для копирования
TEMP_LOG_LIST="/tmp/log_list_$$.txt"
> "$TEMP_LOG_LIST"

# Функция для определения, попадает ли лог-файл в нужный период
# Извлекаем дату из имени файла (формат: sprecv.log-YYYYMMDD или sprecv.log-YYYYMMDD.gz)
check_log_date() {
    local filename=$(basename "$1")
    local date_part=$(echo "$filename" | sed -n 's/.*sprecv\.log-\([0-9]\{8\}\).*/\1/p')
    
    if [ -z "$date_part" ]; then
        # Текущий лог без даты в имени
        return 0
    fi
    
    # Преобразуем дату в timestamp для сравнения
    local log_date="${date_part:0:4}-${date_part:4:2}-${date_part:6:2}"
    local log_timestamp=$(date -d "$log_date" +%s 2>/dev/null)
    
    # Проверяем, попадает ли в период
    if [ -n "$log_timestamp" ] && [ $log_timestamp -ge $start_timestamp ] && [ $log_timestamp -le $end_timestamp ]; then
        return 0
    fi
    
    return 1
}

# Находим все подходящие лог-файлы
for logfile in "$SOURCE_LOG_DIR"/sprecv.log*; do
    if [ -f "$logfile" ]; then
        if check_log_date "$logfile"; then
            echo "$logfile" >> "$TEMP_LOG_LIST"
        fi
    fi
done

# Копируем найденные файлы
copied_count=0
while read logfile; do
    if [ -n "$logfile" ]; then
        cp "$logfile" "$LOG_DIR/" 2>/dev/null
        if [ $? -eq 0 ]; then
            copied_count=$((copied_count + 1))
        fi
    fi
done < "$TEMP_LOG_LIST"

echo "Скопировано файлов: $copied_count"

# Распаковываем .gz файлы
echo "Распаковка архивов..."
unzipped_count=0
for gzfile in "$LOG_DIR"/sprecv.log*.gz; do
    if [ -f "$gzfile" ]; then
        gunzip -f "$gzfile" 2>/dev/null
        if [ $? -eq 0 ]; then
            unzipped_count=$((unzipped_count + 1))
        fi
    fi
done

echo "Распаковано архивов: $unzipped_count"
echo ""

# Шаг 2: Парсим логи и строим список периодов активности
echo "Шаг 2: Анализ логов для определения периодов активности..."
TEMP_EVENTS="/tmp/events_$$.txt"
TEMP_PERIODS="/tmp/periods_$$.txt"

# Очищаем временные файлы
> "$TEMP_EVENTS"
> "$TEMP_PERIODS"

# Ищем все события ev=3299 в логах и извлекаем timestamp и val
cat "$LOG_DIR"/sprecv.log* 2>/dev/null | grep "ev=3299" | grep -v "_res" | while read line; do
    # Извлекаем timestamp из received(...)
    timestamp=$(echo "$line" | sed -n 's/.*received(\([0-9]*\)\..*/\1/p')
    # Извлекаем значение val
    val=$(echo "$line" | sed -n 's/.*val=\([0-9]*\).*/\1/p' | head -1)
    
    if [ -n "$timestamp" ] && [ -n "$val" ]; then
        echo "$timestamp $val" >> "$TEMP_EVENTS"
    fi
done

# Сортируем события по времени
sort -n "$TEMP_EVENTS" > "${TEMP_EVENTS}.sorted"

# Определяем начальное состояние ПГИ по первой записи в логах
first_line=$(head -n 1 "${TEMP_EVENTS}.sorted")
first_val=$(echo "$first_line" | awk '{print $2}')

# Строим периоды активности (от val=1 до val=0)
current_state=0
period_start=0

# Если первая запись val=1, значит ПГИ был включен до начала логов
# Добавляем период от начала анализа до первого события
if [ "$first_val" == "1" ]; then
    # ПГИ был включен, значит считаем роспуски с начала периода
    current_state=1
    period_start=$start_timestamp
else
    # ПГИ был выключен, значит НЕ считаем роспуски до первого события
    current_state=0
fi

while read timestamp val; do
    if [ "$val" == "1" ] && [ "$current_state" == "0" ]; then
        # Начало активного периода (ПГИ включился)
        period_start=$timestamp
        current_state=1
    elif [ "$val" == "0" ] && [ "$current_state" == "1" ]; then
        # Конец активного периода (ПГИ выключился)
        echo "$period_start $timestamp" >> "$TEMP_PERIODS"
        current_state=0
    fi
done < "${TEMP_EVENTS}.sorted"

# Если последнее состояние было активным, закрываем период текущим временем
if [ "$current_state" == "1" ]; then
    current_time=$(date +%s)
    echo "$period_start $current_time" >> "$TEMP_PERIODS"
fi

# Подсчитываем количество активных периодов
period_count=$(wc -l < "$TEMP_PERIODS")
echo "Найдено активных периодов: $period_count"
echo ""

current_timestamp=$start_timestamp

while [ $current_timestamp -lt $end_timestamp ]; do
    # Конец текущей смены
    shift_end=$((current_timestamp + SHIFT_DURATION))
    
    # Формируем читаемую дату и время
    shift_date=$(date -d "@$current_timestamp" "+%d %B %Y %H:%M:%S")
    
    # Строим SQL условие для учета активных периодов
    where_periods=""
    
    if [ -s "$TEMP_PERIODS" ]; then
        # Есть активные периоды - строим условие
        first=1
        while read period_start period_end; do
            # Находим пересечение периода активности со сменой
            active_start=$period_start
            active_end=$period_end
            
            # Ограничиваем период рамками смены
            if [ $active_start -lt $current_timestamp ]; then
                active_start=$current_timestamp
            fi
            if [ $active_end -gt $shift_end ]; then
                active_end=$shift_end
            fi
            
            # Проверяем, есть ли пересечение
            if [ $active_start -lt $active_end ]; then
                if [ $first -eq 1 ]; then
                    where_periods="(time >= $active_start AND time < $active_end)"
                    first=0
                else
                    where_periods="$where_periods OR (time >= $active_start AND time < $active_end)"
                fi
            fi
        done < "$TEMP_PERIODS"
        
        if [ -n "$where_periods" ]; then
            # Есть активные периоды в этой смене
            count=$(mysql -h "$DB_HOST" -u "$DB_USER" -p"$DB_PASS" -D "$DB_NAME" -N -B -e \
                "SELECT COUNT(DISTINCT nom_prot) FROM rosp_stat WHERE ($where_periods);")
        else
            # Нет активных периодов в этой смене
            count=0
        fi
    else
        # Нет информации об активных периодах - считаем все роспуски
        count=$(mysql -h "$DB_HOST" -u "$DB_USER" -p"$DB_PASS" -D "$DB_NAME" -N -B -e \
            "SELECT COUNT(DISTINCT nom_prot) FROM rosp_stat WHERE time >= $current_timestamp AND time < $shift_end;")
    fi
    
    # Определяем тип смены
    hour=$(date -d "@$current_timestamp" "+%H")
    if [ "$hour" == "08" ]; then
        shift_type="(дневная смена)"
    else
        shift_type="(ночная смена)"
    fi
    
    # Получаем правильное склонение слова "роспуск"
    rosp_word=$(get_rosp_word $count)
    
    # Выводим результат
    echo "$shift_date $shift_type - $count $rosp_word"
    
    # Переходим к следующей смене
    current_timestamp=$shift_end
done

echo ""
echo "Анализ завершен"

# Шаг 3: Очистка временных файлов
echo ""
echo "Шаг 3: Очистка временных файлов..."
rm -f "$TEMP_EVENTS" "${TEMP_EVENTS}.sorted" "$TEMP_PERIODS" "$TEMP_LOG_LIST"

# Удаляем скопированные лог-файлы из /tmp
cleaned_count=0
for logfile in "$LOG_DIR"/sprecv.log*; do
    if [ -f "$logfile" ]; then
        rm -f "$logfile"
        cleaned_count=$((cleaned_count + 1))
    fi
done

echo "Удалено временных файлов из $LOG_DIR: $cleaned_count"
echo ""
echo "Все операции завершены!"

