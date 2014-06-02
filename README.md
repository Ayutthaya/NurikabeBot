To get a grid from http://fr.puzzle-nurikabe.com/ :

wget http://fr.puzzle-nurikabe.com/ ;
grep nun.gif index.html | sed -e "s/.*<tbody>//" | sed -e "s/<\/tbody.*//" | sed -e "s/<\/tr>/}, /g" grid.txt | sed -e "s/<tr>/{/g" | sed -e "s/<td>[^>]*><\/td>/0, /g" | sed -e "s/<td>//g" | sed -e "s/<\/td>/, /g"

