   #da finire
    for arg in "$@";do
        #ifper controllo se nome file da scaricare è valido
        curl -LJO https://raw.githubusercontent.com/Pit-Red/progetto_navi/main/$i;

    i=$((i + 1));
done