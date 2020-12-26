# Chess AI
## Introductie
De laatste tijd ben ik (samen met veel anderen) gefascineerd door schaken. Hierdoor ben ik mezelf wat schaakstrategieën beginnen aanleren waardoor ik kennis heb gemaakt met de schaak-engine Stockfish.

Ik begon geïntrigeerd te geraken door de werking van zo'n schaak-engine, dus ben ik wat opzoekwerk gaan uitvoeren. Het leek mij interessant om zelf op lange termijn een schaak-engine te schrijven. Maar eerst moest ik beginnen met een iets realistischer doel dan meteen een neuraal net te coderen, namelijk een schaak-AI maken met het gebruik van het Minimax algoritme en Alpha-Beta pruning.

## Minimax
Minimax is een zoek-algoritme die het programma toelaat om naar toekomstige situaties te kijken om te beslissen welke stappen die moet ondernemen die uiteindelijk het meeste in zijn voordeel zijn.

Dit algoritme werkt op basis van een vooraf bepaalde diepte zodat het programma weet hoe ver in de toekomst er gekeken moet worden.

Je kan dit visueel voorstellen als een search tree waarbij elke node een schaakpositie is. Om het simpel te houden, ga ik het voorstellen met een binary search tree.

We beginnen bij de root node, wat de huidige positie voorstelt:
