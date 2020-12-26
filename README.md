# Chess AI
## Introductie
De laatste tijd ben ik (samen met veel anderen) gefascineerd door schaken. Hierdoor ben ik mezelf wat schaakstrategieën beginnen aanleren waardoor ik kennis heb gemaakt met de schaak-engine Stockfish.

Ik begon geïntrigeerd te geraken door de werking van zo'n schaak-engine, dus ben ik wat opzoekwerk gaan uitvoeren. Het leek mij interessant om zelf op lange termijn een schaak-engine te schrijven. Maar eerst moest ik beginnen met een iets realistischer doel dan meteen een neuraal net te coderen, namelijk een schaak-AI maken met het gebruik van het Minimax algoritme en Alpha-Beta pruning.

## Minimax
Minimax is een zoek-algoritme die het programma toelaat om naar toekomstige situaties te kijken om te beslissen welke stappen die moet ondernemen die uiteindelijk het meeste in zijn voordeel zijn.

Dit algoritme werkt op basis van een vooraf bepaalde diepte zodat het programma weet hoe ver in de toekomst er gekeken moet worden.

Je kan dit visueel voorstellen als een search tree waarbij elke node een schaakpositie is. Om het simpel te houden, ga ik het voorstellen met een binary search tree.

We beginnen bij de root node, wat de huidige positie voorstelt:

![](Images/Root.png)

Stel onze AI speelt met de zwarte stukken en het is nu aan de AI om te spelen. Om die een beslissing te laten maken, gaan we het Minimax algoritme oproepen met de huidige positie en een bepaalde diepte.

We zullen hier als diepte 2 nemen. Dit betekent dus dat de AI 2 stappen vooruit in de toekomst zal gaan kijken. Eens de diepte bereikt hebben, gaan we alle posities tot die diepte evalueren op basis van een evaluatie-functie, bv. de mogelijkheid om de tegenstander zijn stukken te nemen.

Zwart heeft vanuit de huidige positie verschillende opties:

![](Images/Minimax_tree.PNG)

Het eerste dat er gebeurt is de linkse kant van de tree traverseren tot we de opgegeven diepte behaald hebben (2). Eens we de diepte behaald hebben, gaan we alle mogelijke posities evalueren. Dit doe ik door het totaal van de zwarte score af te trekken van het totaal van de witte score. (de score van een kleur is het totaal van de waardes van de overblijvende stukken) 

Elk stuk heeft een vaste waarde: pion 10, paard en loper 30, toren 50, koningin 90 en koning 900.

Als de evaluatie-functie een negatieve waarde returned, betekent dat dat zwart (minimizing player) in het voordeel is. Een positieve waarde betekent dan dat wit in het voordeel is. Dus als we op een laag zijn waar het aan wit is om te bewegen, gaat wit de meest positieve evaluatie kiezen. Als het aan zwart is, gaat zwart de meest negatieve evaluatie kiezen.

Eens we alle posities in de tree hebben geëvalueerd, gaan we dus onze AI de positie laten kiezen die voor hem het meeste voordelig is. (in onze situatie is de AI zwart dus die gaat de meest negatieve evaluatie kiezen). Dat is het Minimax algoritme.

## Alpha-Beta pruning
Alpha-Beta pruning is het niet evalueren van bepaalde nodes in de tree waardoor het Minimax algoritme sneller zal verlopen.

