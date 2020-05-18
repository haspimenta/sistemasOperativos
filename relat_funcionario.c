#!/bin/bash

(( REFERENCE_TIME=8*3600 ))

# Declara arrays associativos
declare -A reg_name mvt_dates mvt_by_date

exec 3< matricula.txt
while read reg name <&3; do
  reg_name[$reg]="$name";
done
exec 3<&-

exec 3< ponto.txt
while read line <&3; do
  if [[ ${#line} = 31 ]] && [[ "${line/*[^0-9]*/}" = "$line" ]]; then
    mvt_code=${line:0:12}
    reg=${line:12:7}
    year=$((2000+${line:19:2}))
    month=${line:21:2}
    day=${line:23:2}

    # Abaixo eu separo partes da hora, de modo inicialmente
    # parecido com as separações acima, mas note que eu fui
    # obrigado a acrescentar código para remover o algarismo
    # zero antes de valores como "08" e "09".  Sem isso, ao
    # encontrar o de um número, o Bash entende que o restante
    # do número está em base octal, e dá erro ao encontrar um
    # algarismo 8 ou 9, que não são válidos nessa base.
    hour=${line:25:2}
    hour=${hour#0}
    minute=${line:27:2}
    minute=${minute#0}
    second=${line:29:2}
    second=${second#0}

    if [[ ${mvt_code:0:2} = "01" ]]; then
      if [[ ${mvt_dates[$year$month$day]-UNSET} = UNSET ]]; then
        mvt_dates[$year$month$day]=""
      fi
      mvt_by_date[$year$month$day,$reg]=$((3600*hour+60*minute+second))
    fi
  fi
done
exec 3<&-


#### Imprime resultado, devidamente separado por data.

# Note que tem de chamar comandos externos só para ordenar
# as chaves de pesquisa, pois o shell não tem como fazer
# por conta própria.
exec 3< <( echo "${!mvt_dates[@]}" | tr " " \\n | sort -u )
while read date <&3; do
  year=${date:0:4}
  month=${date:4:2}
  day=${date:6:2}
  echo "Relatório do dia $day/$month/$year:"
  exec 4< <( echo "${!reg_name[@]}" | tr " " \\n | sort -u )
  while read reg <&4; do
    printf "\t%7.7s - %-30.30s - " "$reg" "${reg_name[$reg]}"
    if [[ ${mvt_by_date[$date,$reg]+SET} = SET ]]; then
      time=${mvt_by_date[$date,$reg]}
      ((second=time%60))
      ((minute=((time-second)/60)%60))
      ((hour=(time-60*minute-second)/3600))
      printf "%02d:%02d:%02d - " "$hour" "$minute" "$second"
      if (( time<=REFERENCE_TIME )); then
        echo "Presente"
      else
        echo "ATRASADO"
      fi
    else
      echo "--:--:-- - AUSENTE"
    fi
  done
  exec 4<&-
  printf "\\f"
done
exec 2<&-
