Author: Walid nakad
link:https://github.com/Crash-E/Zork.



# ⚔️ Cave of Excalibur

**A text‑adventure dungeon crawler based on king Arthur.**  
Enter the cave. Face its trials. Draw the sword—if you are worthy.

---

## 📖 Backstory

The cave has always been there, half‑hidden in the hills, older than memory.  
They say King Arthur’s spirit still guards the sword he once pulled from the stone.  
They say the cave judges those who enter—not by strength of arm, but by the choices they make.

You are the latest to try.

Inside, the walls shift. Statues watch. Knights, cultists, and desperate souls wander the dark.  
At the heart of the labyrinth lies Excalibur, sheathed in a shimmering forcefield, waiting for a hand pure enough to claim it.

But the cave does not give up its treasures lightly.  
Some seekers never return.  
Others wake from a dream to find the sword was never theirs.  
Only a few walk out legends.

---

## 🎮 Mechanics

### Stats & Progression

| Stat | Effect |
|------|--------|
| **HP** | Life force. When it hits 0, your story ends. |
| **Fight** | Number of coin flips when attacking. |
| **Act**   | Coin flips for non‑violent actions (talk, yield, offer). |
| **Flee**  | Coin flips when attempting to escape combat. |
| **Item**  | Coin flips when using an item under pressure. |
| **Evade** | Coin flips to block incoming enemy attacks. |
| **Worthiness** | Hidden score (0–10). Raised by compassion, lowered by cruelty. |

- Level up by gaining XP. Choose one stat to increase each time.
- Equip weapons and armour to boost your stats.

### Combat

Combat is **turn‑based** with coin‑flip success chances:

- `attack` – flip Fight coins; any head hits.
- `act` – flip Act coins; success triggers special NPC reactions.
- `item` – flip Item coins; success lets you use an item mid‑battle.
- `flee` – flip Flee coins; success escapes (but you retreat).
- `status` – view your stats (no turn used).

**Sneak mode** (toggle with `sneak`).
get an extra turn at the start of combat or gain 50% more xp at the end of each fight.

### Worthiness

Your hidden moral compass. Starting value: 10.

- Lose worthiness by stealing, killing, or selfish deeds.
- Your final worthiness determines which of the 7 endings you unlock.

### Items & Treasure

- Most items have a **gold value**. Some are wearable (buff stats), others are consumable (heal, damage, utility).
- The **Holy Grail** has special power: it can shatter illusions.
- **Examine everything**—secrets are often hidden behind a closer look.

### Random elements

-roaming boar enemy that stalks the early rooms of the dungeon (RA rooms)
-randomized room placement. each run the rooms order that you encounter might change

### 10 different endings

-7 core endings, 1 secret ending, and 2 variations of the 7 endings

### NPC Bossfights and sidequests

-various npcs littered throughout the game. unique interactions with all.

---

## 🗺️ Map

The cave layout is **partially randomised** each game. Below is the fixed structure.


                   Excalibur                                                              pitbottom connected to pit room in RA
                      |                                            
                     RB2-secret room    RA3                          			  no connection = dream room
                      |                  |                         
                     RB1-Altar Entrance-RA2-RA1                                           RA: Waterfall/narrow tunnel/pit room
                                             |                                                                                 RA and RB rooms randomize places each run.
                                        Cave entrance                                     RB: catacombs/ritual room
                                             |
                                            exit



### walkthrough

excalibur puzzle: follow the gazes of the statue, then interact with the runes. solution= A G B

secret room: after inspecting the arthur statue in excalibur room, go back to altar entrance and find a hidden lever to open the door in RB2

injured man can be given yellow herbs, green herbs and potion. each has a different interaction

graverobber can be calmed by robbing the 2 of the 4 coffins

boar can be sedated with yellow herbs

if you have the dagger, cultists can be calmed through act option

yield twice to peaceful resolve the green knight encounter

give merlin the holy grail to end the fight with him

use rope on the pit

use the crystal ball in merlins room to check your worthiness

inspect everything

### endings:

all endings are accessible after touching sword, whether worthy or not

be unworthy for 7/7

leave without excalibur with value > 20 for ending 6/7

grab sword with worthiness between 6 9 to get one of the dream ending endings 5/7. have holygrail for variations 5/7-s and 6/7-s.

grab sword with worthiness 10 then leave through exit to get ending 4/7. 

have value >20 with excalibur to get ending 3/7

grab excalibur with -1 worthiness for 2/7

have grail, -1 worthiness, and grab excalibur for ending 1/7

secret ending ?/7. go south at the start of the game.


### extra lore:

in this story king arthur was killed by mordred. and due to her grief the lady of the lake has put a curse on excalibur.






MIT License

Copyright (c) [2026] [Walid Nakad]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
