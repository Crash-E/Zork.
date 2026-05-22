#include "WorldBuilder.h"
#include "World.h"
#include "Room.h"
#include "Item.h"
#include "Exit.h"
#include "NPC.h"
#include "Creature.h"
#include "Player.h"
#include "Utils.h"
#include "Combat.h"
#include "Commands.h"
#include "Endings.h"
#include <iostream>
#include <vector>
#include <algorithm>

void WorldBuilder::Build(World& world) {
	const int rich = 20;

	// ROOMS
	Room* entrance = new Room("Cave Entrance",
		"You stand at the mouth of a dark cave. A cold wind blows from within.\n"
		"The light of day barely reaches past the first few feet of stone.\n"
		"Yellow plants cover the fields to your right.");

	Room* AltarEntrance = new Room("Altar Entrance",
		"The tunnel opens into a grand stone gate, its archway carved with scenes of battles long forgotten.\n"
		"Two statues flank the entrance.\n"
		"To the right, a robed figure clutching a staff, his face worn smooth by time.\n"
		"To the left, a broken warrior, sword in hand, stands vigil, missing its head and left arm.\n"
		"You stumble upon a bundle of ropes on the ground.");

	Room* ExcaliburRoom = new Room("Excalibur Chamber",
		"The chamber opens into a vast circular room, its ceiling lost in darkness above.\n"
		"At its center, a sword stands embedded in a weathered stone, surrounded by a shimmering forcefield that distorts the air around it.\n"
		"Three runes are carved into the base of the stone, each glowing faintly - A, G, and B.\n"
		"Two knight statues flank the stone. The first, tall and noble, faces the second.\n"
		"The second turns its gaze toward the sword, as if it has been watching over it for centuries.\n"
		"Behind them both, a larger statue of a crowned figure stares toward the first knight, its expression unreadable.");

	Room* Dream = new Room("Dream",
		"The cave gives way to something impossible. You find yourself in an endless grassfield.\n"
		"An open sky stretches above you, pale and still, stars unmoving as if painted there.\n"
		"A calm lake spreads before you, its surface like glass, reflecting nothing.\n"
		"At its center, a figure stands upon the water, draped in white, watching you in silence.");

	Room* Quarters = new Room("Quarters",
		"A narrow passage opens into a hidden chamber, warm and dry compared to the cave outside.\n"
		"Shelves line the walls, crowded with scrolls, tomes, and artifacts wrapped in cloth.\n"
		"Someone has been living here. A bedroll sits in the corner, a half eaten meal beside it.\n"
		"At the center of the room, a stone pedestal holds a single golden cup, glowing faintly in the dark.");

	Room* PitRoom = new Room("Pit Room",
		"A wide chamber with a deep pit at its center.\n"
		"Strange herbs grow in clusters along the walls.\n"
		"You can't tell how deep the pit is just by looking.");

	Room* PitBottom = new Room("Pit Bottom",
		"You reach the bottom of the pit. It's cold and damp.\n"
		"On a flat stone sits a single silver gauntlet, untouched by time.");

	Room* NarrowTunnel = new Room("Narrow Tunnel",
		"The passage narrows until your shoulders nearly brush the walls on both sides.\n"
		"The air is stale and still. Nothing moves here.\n"
		"A skeleton slumps against the wall ahead, one hand clutching something to its chest.\n"
		"A worn leather sheath at its waist holds a dagger that catches the torchlight - oddly clean for this place.");

	Room* Waterfall = new Room("Waterfall",
		"The cave opens into a wide grotto, the sound of rushing water filling the air.\n"
		"Cracks in the ceiling feed a steady stream that cascades down the far wall into a shallow pool below.\n"
		"Herbs grow along the water's edge, fed by the mist.\n"
		"Slumped against a rock near the pool, a man lies motionless, his chest rising and falling slowly.");

	Room* Catacombs = new Room("Catacombs",
		"The walls here are not stone but coffins, stacked from floor to ceiling in every direction.\n"
		"Skulls line the gaps between them, their hollow eyes watching your every step.\n"
		"The air is thick and cold.\n"
		"A figure crouches near the far wall, rifling through a coffin, freezing the moment he hears you enter.");

	Room* RitualRoom = new Room("Ritual Room",
		"The smell hits you before you see it - incense, thick and sweet, masking something underneath.\n"
		"Three robed figures stand in a circle at the room's center, their backs to you.\n"
		"Candles flicker across strange symbols carved into the floor.\n"
		"As one, they turn.");

	world.AddEntity(entrance);
	world.AddEntity(AltarEntrance);
	world.AddEntity(ExcaliburRoom);
	world.AddEntity(Dream);
	world.AddEntity(Quarters);
	world.AddEntity(PitBottom);
	world.AddEntity(PitRoom);
	world.AddEntity(NarrowTunnel);
	world.AddEntity(Waterfall);
	world.AddEntity(Catacombs);
	world.AddEntity(RitualRoom);

	// CAVE ENTRANCE CONTENT
	Item* poisonHerbs = new Item("Yellow Herbs",
		"A cluster of yellow herbs. Often used by hunters for traps.",
		"Herbs used to sedate beasts during hunts. Strong enough to knock a person out for hours.",
		false, 1, "eat the herbs");

	NPC* mordred = new NPC("Mordred",
		"A figure in dark armor stands at the cave mouth, blocking your path.",
		100, true, entrance,
		"I won't allow my uncle to have a successor. Prepare to die!");
	
	mordred->onTalk = [mordred](Player* p, World* w) {
		std::cout << "\"You think you can just walk out of here?\"\n";
		std::cout << "Mordred's voice is cold and filled with rage.\n";
		std::cout << "!!!===YOU CAN'T ESCAPE===!!!\n";
		p->Flee = -5;
		PressEnter();
		bool hasExcalibur = false;
		for (Item* item : p->inventory) {
			if (item->name == "Excalibur") { hasExcalibur = true; break; }
		}
		if (hasExcalibur) {
			std::cout << "Mordred's eyes widen as he sees the sword in your hands.\n";
			std::cout << "\"No... how did you get that?\"\n";
			std::cout << "He looks at you with a mix of fear and hatred.\n";
			std::cout << "\"I won't let you take the sword. You don't deserve it.\"\n";
		}

	};
	mordred->onAct = [mordred](Player* p, World* w) {
		static int actCount = 0;

		// Check for Arthur's Ring
		Item* ring = nullptr;
		for (Item* item : p->inventory) {
			if (item->name == "Arthur's Ring") { ring = item; break; }
		}

		if (ring && actCount == 0) {
			actCount++;
			std::cout << "You raise Arthur's seal.\n";
			std::cout << "Mordred's eyes widen. \"That ring... where did you-\"\n";
			std::cout << "His grip falters.\n";
			mordred->damage -= 8;
			std::cout << "(Mordred is shaken.)\n";
		}
		else if (actCount == 0) {
			std::cout << "You try to reason with him. He only laughs.\n";
			std::cout << "\"Words won't save you here.\"\n";
			actCount++;
		}
		else {
			std::cout << "He has heard enough. The talking is done.\n";
		}
	};
	mordred->damage = 18;
	mordred->worthCost = 0;
	world.AddEntity(mordred);

	entrance->AddEntity(poisonHerbs);
	world.AddEntity(poisonHerbs);

	// ALTAR ENTRANCE CONTENT
	Item* leverStatue = new Item("Broken Statue",
		"A broken statue stands guard.",
		"The pose matches the Arthur statue in the excalibur chamber.\na lever besides it looks operable.",
		false, 0, "pull the lever", true);
	world.AddEntity(leverStatue);

	Item* Rope = new Item("Rope",
		"A cluster of ropes, always useful.", "", false, 2);

	AltarEntrance->AddEntity(Rope);
	world.AddEntity(Rope);

	NPC* greenKnight = new NPC("Green Knight",
		"A towering knight clad in green armor, his axe resting on his shoulder.",
		80, true, AltarEntrance,
		"You dare approach the gate? State your purpose or prepare to fight.");
	greenKnight->damage = 15;
	greenKnight->worthCost = 3;

	greenKnight->onAct = [greenKnight, AltarEntrance](Player* p, World* w) {
		std::cout << "\"This is no time to talk, yield or fight!\"\n";
		std::cout << "[yield] [fight]\n> ";
		std::string choice;
		std::getline(std::cin, choice);
		if (choice == "yield" || choice == "y") {
			greenKnight->actCount++;
			if (greenKnight->actCount == 1) {
				std::cout << "\"Interesting. You hesitate. Show me more.\"\n";
				std::cout << "The knight steps back, but his axe is still raised.\n";
				PressEnter();
			}
			else {
				std::cout << "You lower your weapon. The knight nods slowly.\n";
				std::cout << "\"Wisdom. You may pass.\"\n";
				std::cout << "The knight's ghostly figure begins to fade away.\n";
				greenKnight->isHostile = false;
				greenKnight->peacefulResolved = true;
				greenKnight->hp = 0;
				AltarEntrance->RemoveEntity(greenKnight);
				PressEnter();
			}
		}
		else if (choice == "fight" || choice == "f") {
			std::cout << "You prepare to fight the Green Knight.\n";
		}
		else {
			std::cout << "The Green Knight respects your choice.\n";
		}
		};

	AltarEntrance->AddEntity(greenKnight);
	world.AddEntity(greenKnight);

	// Pit Bottom contents
	Item* pristineGauntlet = new Item("silver Gauntlet",
		"A silver gauntlet sits on the stone.",
		"Looks like it was placed here yesterday. The metal is untouched.", false, 3);
	PitBottom->AddEntity(pristineGauntlet);
	world.AddEntity(pristineGauntlet);

	// Pit Room contents
	Item* medHerbs = new Item("green Herbs",
		"A cluster of green herbs.", "often used by alchemists to treat wounds. (heals 50 hp)", false, 1, "eat the herbs");
	PitRoom->AddEntity(medHerbs);
	world.AddEntity(medHerbs);

	// NARROW TUNNEL CONTENT
	Item* skeleton = new Item("Skeleton",
		"A skeleton slumps against the wall.",
		"Old brittle bones, armor rusted to nothing. His hand seems to be clutching something. You notice a sheath around his waist.",
		false, 0, "", true);

	Item* hand = new Item("Skeleton's Hand",
		"The skeleton's hand seems to be clutching something.",
		"The fingers are curled tight around something red, a crumpled note wedged beneath the thumb.",
		true, 0, "", true);

	Item* sheath = new Item("Sheath",
		"A worn leather sheath at its waist.",
		"Cracked leather, but the blade inside catches the light perfectly - untouched by time.",
		true, 0, "", true);

	Item* potion = new Item("Potion",
		"A small vial of red liquid. Looks like it could heal wounds.", "a valuable elixir, allowing a person to receive more damage than they can take! (hp = 150)", false, 2);

	Item* wornNote = new Item("Worn Note",
		"A crumpled piece of paper. The ink is smudged beyond repair.",
		"Most of the note is illegible, you can make out \"make sure to st*- safe. love Mari*-\"",
		false, 0);

	Item* pristineDagger = new Item("Pristine Dagger",
		"A dagger in perfect condition. Oddly clean for such a dark place.", "A dagger with runes carved on its blade. whoever owned this must have been very wealthy. (+2 fight)", false, 3);
	pristineDagger->buffFight = 2;

	hand->AddEntity(potion);
	hand->AddEntity(wornNote);
	sheath->AddEntity(pristineDagger);

	NarrowTunnel->AddEntity(skeleton);

	skeleton->onExamine = [hand, sheath, NarrowTunnel](Player* p) {
		static bool revealed = false;
		if (!revealed) {
			NarrowTunnel->AddEntity(hand);
			NarrowTunnel->AddEntity(sheath);
			revealed = true;
		}
		};

	world.AddEntity(skeleton);
	world.AddEntity(hand);
	world.AddEntity(sheath);
	world.AddEntity(potion);
	world.AddEntity(wornNote);
	world.AddEntity(pristineDagger);

	// CATACOMBS CONTENT
	Item* bejeweledSkull = new Item("Bejeweled Skull",
		"A skull encrusted with jewels.", "who would do something like this to a skull", false, 5);
	Item* ornamentalSword = new Item("Ornamental Sword",
		"An ornate sword. Looks ceremonial, not practical.", "holding this sword is awkward, still better than nothing (+1 fight, +1 Act)", false, 5);
	Item* ancientCloak = new Item("Ancient Cloak",
		"A cloak woven with strange symbols.", "good for concealing oneself or surprise attacks (+1 flee, +1 item)", false, 4);
	Item* burnedGrimoire = new Item("Burned Grimoire",
		"A burnt book. One line is still legible.", "You read Out loud \"KLAATU, VERATA, NECKTIE\"\n...\n...\nnothing happens, maybe you read it wrong?", false, 4);
	world.AddEntity(bejeweledSkull);
	world.AddEntity(ornamentalSword);
	world.AddEntity(ancientCloak);
	world.AddEntity(burnedGrimoire);

	static std::vector<Item*> coffinLoot = { bejeweledSkull, ornamentalSword, ancientCloak, burnedGrimoire };
	std::shuffle(coffinLoot.begin(), coffinLoot.end(), world.rng);

	Item* coffins = new Item("Coffins",
		"Four stone coffins are stacked along the walls.",
		"Four ancient coffins. Some look disturbed already.",
		false, 0, "loot a coffin", true);

	NPC* graveRobber = new NPC("Grave Robber",
		"A thin man crouches in the shadows, knife in hand. \nhe eyes you suspiciously, better not approach.",
		50, false, Catacombs,
		"");
	graveRobber->damage = 10;
	graveRobber->worthCost = 2;
	graveRobber->onTalk = [graveRobber](Player* p, World* w) {
		if (!graveRobber->peacefulResolved) {
			std::cout << "The grave robber looks at you. \"should have left me alone!\"\n he takes a combat position.\n";
			Combat(*p, *graveRobber, *w);
		}
		else {
			std::cout << "I heard king arthur was buried here after he died. a shame he got betrayed.";
		}
		};

	static std::vector<int> coffinState = { 0, 0, 0, 0 };

	coffins->onInteract = [coffins, graveRobber, Catacombs](Player* p) {
		std::cout << "Which coffin do you loot? (1/2/3/4)\n> ";
		std::string choice;
		std::getline(std::cin, choice);
		int idx = -1;
		if (choice == "1") idx = 0;
		else if (choice == "2") idx = 1;
		else if (choice == "3") idx = 2;
		else if (choice == "4") idx = 3;
		else { std::cout << "Not a coffin number.\n"; return; }

		if (coffinState[idx] != 0) {
			std::cout << "That coffin is already empty.\n";
			return;
		}

		Item* loot = coffinLoot[idx];
		p->inventory.push_back(loot);
		p->TotalValue += loot->Value;
		coffinState[idx] = 1;
		std::cout << "You loot the coffin and find: " << loot->name << ".\n";

		if (graveRobber->IsAlive()) {
			for (int i = 3; i > 0; --i) {
				if (coffinState[i] == 0) {
					Item* robberLoot = coffinLoot[i];
					graveRobber->drops.push_back(robberLoot);
					coffinState[i] = 2;
					std::cout << "The grave robber opens another coffin, stuffing its content into his bag.\n";
					break;
				}
			}
		}

		bool allEmpty = true;
		for (int s : coffinState) if (s == 0) { allEmpty = false; break; }
		if (allEmpty && graveRobber->IsAlive()) {
			graveRobber->peacefulResolved = true;
			graveRobber->description = "A fellow graverobber.";
			std::cout << "\nthe grave robber relaxes at your unified intentions. \"I guess I'm not the only one after the kings tomb\"\n";
		}
		};

	Item* loneSkull = new Item("Lone Skull",
		"A single skull sits in the middle of the room, alone.",
		"An ordinary skull, oddly placed in the center of the room.",
		false, 0);
	loneSkull->onTake = [loneSkull, Catacombs](Player* p) {
		p->inventory.remove(loneSkull);
		std::cout << "The skull crumbles to dust the moment you touch it.\n";
		};

	Catacombs->AddEntity(coffins);
	Catacombs->AddEntity(graveRobber);
	Catacombs->AddEntity(loneSkull);
	world.AddEntity(coffins);
	world.AddEntity(graveRobber);
	world.AddEntity(loneSkull);

	// Roaming Boar
	Creature* Boar = new Creature("Boar", "A wild boar, tusks gleaming.", 40, true);
	Boar->damage = 7;
	Boar->worthCost = 1;
	world.AddEntity(Boar);
	Boar->onAct = [Boar](Player* p, World* w) {
		Item* herbs = nullptr;
		for (Item* item : p->inventory) {
			if (item->name == "Yellow Herbs") { herbs = item; break; }
		}
		if (!herbs) {
			std::cout << "The boar snarls. There is nothing you can do. if you only had any hunting experience.\n";
			return;
		}
		std::cout << "Try to sedate the boar with Yellow Herbs? (y/n)\n> ";
		std::string choice;
		std::getline(std::cin, choice);
		if (choice != "y" && choice != "yes") return;

		if (CoinFlip(p->ItemStat, "applying", "fumbling", "sedating", *w)) {
			std::cout << "The boar staggers, then collapses, snoring softly.\n";
			Boar->name = "Sleeping Boar";
			Boar->damage = 0;
			p->inventory.remove(herbs);
			p->TotalValue -= herbs->Value;
			Boar->isHostile = false;
			Boar->peacefulResolved = true;
			Boar->combatEnded = true;
			Boar->patrolRooms.clear();
		}
		else {
			std::cout << "You fail to apply the herbs! The boar snorts angrily.\n";
		}
		};
	Item* boarMeat = new Item("Boar Meat", "Tough but edible meat.", "Fresh boar meat. (heals 20hp)", false, 0);
	Item* tusks = new Item("Tusks", "Sharp boar tusks.", "valuable to hunters and shamans alike", false, 2);
	world.AddEntity(boarMeat);
	world.AddEntity(tusks);

	Boar->drops.push_back(boarMeat);
	Boar->drops.push_back(tusks);

	// WATERFALL CONTENT
	NPC* injuredMan = new NPC("Injured Man",
		"A man slumped against a rock, barely conscious.",
		30, false, Waterfall,
		"He seems to be fading in and out of consciousness.");
	injuredMan->damage = 0;
	injuredMan->worthCost = 2;

	Item* food = new Item("Food",
		"Dried meat and bread.", "never venture on an empty stomach! (heals 25 hp)", false, 1, "eat the food");
	world.AddEntity(food);

	Item* bomb = new Item("Bomb",
		"A crude bomb wrapped in cloth.", "my granny got hit by a bazooka... kaboom... kabloaw...", false, 2, "throw the bomb");
	world.AddEntity(bomb);

	Item* rustyShield = new Item("Rusty Shield",
		"An old shield, dented and rusted.", "this will surely be useful at blocking attacks (+2 evade)", false, 2, "raise the shield");
	world.AddEntity(rustyShield);

	Item* leatherArmor = new Item("Leather Armor",
		"Worn leather armor.", "fresh bloodstains cover some of the armor. still good protection (+50 maxhp)", false, 2, "wear the armor");
	world.AddEntity(leatherArmor);

	injuredMan->drops.push_back(food);
	injuredMan->drops.push_back(rustyShield);
	injuredMan->drops.push_back(bomb);
	injuredMan->drops.push_back(leatherArmor);

	Item* arthursRing = new Item("Arthur's Ring",
		"A ring bearing Arthur's seal.", "A simple band bearing Arthur's seal. The metal is warm, as if recently worn. Whoever lost it here was close to him.", false, 3, "wear the ring");
	world.AddEntity(arthursRing);

	Item* medHerbsWf = new Item("green Herbs",
		"A cluster of green herbs.", "often used by alchemists to treat wounds. (heals 50 hp)", false, 1, "eat the herbs");

	Waterfall->AddEntity(medHerbsWf);
	world.AddEntity(medHerbsWf);

	Item* waterfallItem = new Item("Waterfall",
		"Water cascades down the far wall into a shallow pool.",
		"Something glints behind the rushing water.",
		false, 0, "", true);
	waterfallItem->onExamine = [this, waterfallItem, arthursRing](Player* p) {
		std::cout << "You look at the waterfall. The water rushes too fast to see anything clearly.\n";
		if (ringFound) return;
		std::cout << "Look closer at the waterfall? (y/n)\n> ";
		std::string c;
		std::getline(std::cin, c);
		if (c == "y" || c == "yes") {
			std::cout << "You reach behind the water. Your fingers close around something cold and small. A ring.\n";
			p->currentRoom->AddEntity(arthursRing);
			ringFound = true;
		}
		};
	Waterfall->AddEntity(waterfallItem);
	world.AddEntity(waterfallItem);

	Item* bloodiedBody = new Item("Bloodied Body",
		"A man's body lies still, soaked in blood.",
		"Everything on him is ruined. There is nothing of use here.",
		false, 0, "", true);
	world.AddEntity(bloodiedBody);

	injuredMan->onTalk = [injuredMan, food, bomb, rustyShield, leatherArmor, bloodiedBody, Waterfall](Player* p, World* w) {
		if (p->turnsPlayed > 40 && injuredMan->state == 0) {
			Waterfall->RemoveEntity(injuredMan);
			Waterfall->AddEntity(bloodiedBody);
			std::cout << "The man is dead. He must have been gone for a while now.\n";
			return;
		}
		if (injuredMan->state >= 2) {
			std::cout << "Thank you for saving me. I will rest here for a while before going back home.\n";
			PressEnter();
			return;
		}
		std::cout << "The man is fading in and out of consciousness.\n";
		std::cout << "[wake] [give] [steal] [kill]\n> ";
		std::string choice;
		std::getline(std::cin, choice);

		if (choice == "wake") {
			injuredMan->state = 1;
			std::cout << "The man stirs as you wake him.\n\"Please... do you have anything to help?\"\n";
		}
		else if (choice == "give") {
			std::cout << "Give what?\n> ";
			std::string itemName;
			std::getline(std::cin, itemName);
			Entity* item = FindInInventory(p, itemName);
			if (!item) {
				std::cout << "You don't have that.\n";
				return;
			}
			Item* given = static_cast<Item*>(item);

			if (given->name == "Yellow Herbs") {
				std::cout << "He takes the herbs gratefully and drifts into a peaceful sleep.\n";
				std::cout << "He does not wake again.\n";
				p->inventory.remove(given);
				p->TotalValue -= given->Value;
				injuredMan->state = -1;
				Waterfall->RemoveEntity(injuredMan);
				Waterfall->AddEntity(bloodiedBody);
			}
			else if (given->name == "green Herbs") {
				p->inventory.remove(given);
				p->TotalValue -= given->Value;
				p->inventory.push_back(bomb);
				p->TotalValue += bomb->Value;
				auto it = std::find(injuredMan->drops.begin(), injuredMan->drops.end(), bomb);
				if (it != injuredMan->drops.end()) injuredMan->drops.erase(it);
				injuredMan->state = 2;
				injuredMan->hp = 50;
				std::cout << "He eats the herbs and his color returns.\n\"Thank you! Take this. I won't need it now.\"\nHe hands you a bomb.\n";
			}
			else if (given->name == "Potion") {
				p->inventory.remove(given);
				p->TotalValue -= given->Value;
				p->inventory.push_back(rustyShield);
				p->TotalValue += rustyShield->Value;
				auto it = std::find(injuredMan->drops.begin(), injuredMan->drops.end(), rustyShield);
				if (it != injuredMan->drops.end()) injuredMan->drops.erase(it);
				injuredMan->state = 3;
				injuredMan->hp = 150;
				std::cout << "He drinks the potion in one gulp and gets to his feet.\n\"I owe you my life. Take this. Hope it serves you better than me.\"\nHe hands you a rusty shield.\n";
			}
			else {
				std::cout << "He looks at it and shakes his head weakly.\n";
			}
		}
		else if (choice == "steal") {
			p->Worthy -= 1;
			std::cout << "You rifle through his belongings. He doesn't stir.\n";
			std::cout << "You manage to steal some food.\n";
			p->inventory.push_back(food);
			p->TotalValue += food->Value;
			auto it = std::find(injuredMan->drops.begin(), injuredMan->drops.end(), food);
			if (it != injuredMan->drops.end()) injuredMan->drops.erase(it);
		}
		else if (choice == "kill") {
			injuredMan->isHostile = true;
			std::cout << "You raise your weapon.\n";
		}
		PressEnter();
		};

	Waterfall->AddEntity(injuredMan);
	world.AddEntity(injuredMan);

	// RITUAL ROOM CONTENT
	Item* incense = new Item("Incense",
		"A bundle of fragrant incense.", "used to cleanse oneself (restore some worthiness)", false, 3, "burn the incense");
	world.AddEntity(incense);

	Item* ritualisticMask = new Item("Ritualistic Mask",
		"A carved wooden mask with hollow eyes.", "popular among witch doctors. take it for a spin. (+1 act +1 item +1 flee)", false, 5);
	world.AddEntity(ritualisticMask);

	Item* moonPendant = new Item("Moon Pendant",
		"A pendant shaped like a crescent moon.",
		"Cold to the touch. Probably valuable. Tyrande Whisperwind is inscribed on its back.",
		false, 4);
	world.AddEntity(moonPendant);
	RitualRoom->AddEntity(moonPendant);

	Item* ritualisticDrawings = new Item("Ritualistic Drawings",
		"Strange carvings cover the walls.",
		"These carvings depict a lady clad in white cursing a sword. Her tears feed the blade.",
		false, 0, "", true);
	world.AddEntity(ritualisticDrawings);
	RitualRoom->AddEntity(ritualisticDrawings);

	Creature* cultists = new Creature("Cultists",
		"Three robed figures, faces hidden by hoods, chant in unison.",
		70, true, RitualRoom);
	cultists->damage = 12;
	cultists->worthCost = 3;
	cultists->drops.push_back(incense);
	cultists->drops.push_back(ritualisticMask);
	RitualRoom->AddEntity(cultists);
	world.AddEntity(cultists);

	cultists->onAct = [cultists, incense](Player* p, World* w) {
		Item* dagger = nullptr;
		for (Item* item : p->inventory) {
			if (item->name == "Pristine Dagger") { dagger = item; break; }
		}
		if (!dagger) {
			std::cout << "The cultists continue their chant. There is nothing you can do.\n";
			return;
		}
		std::cout << "You raise the Pristine dagger. The cultists pause, watching intently.\n";
		std::cout << "Offer your blood with the Pristine Dagger (-25 hp)? (y/n)\n> ";
		std::string choice;
		std::getline(std::cin, choice);
		if (choice != "y" && choice != "yes") {
			std::cout << "You lower the dagger. They resume their chant.\n";
			return;
		}
		std::cout << "You draw blood with the dagger. The cultists nod approvingly.\n";
		std::cout << "\"It is enough. Leave us to our ritual.\"\n";
		auto it = std::find(cultists->drops.begin(), cultists->drops.end(), incense);
		if (it != cultists->drops.end()) {
			cultists->drops.erase(it);
			std::cout << "one of the cultists silently hands you a bundle of incense.\n";
			p->inventory.push_back(incense);
			p->TotalValue += incense->Value;
		}
		p->TakeDamage(25);
		cultists->isHostile = false;
		cultists->peacefulResolved = true;
		cultists->combatEnded = true;
		};

	// EXCALIBUR CHAMBER CONTENT
	Item* arthurStatue = new Item("Arthur Statue",
		"A grand statue of a crowned figure stands behind the stone.",
		"King Arthur stands tall, crown upon his brow, his stone gaze fixed on the knight to his right.\n"
		"There's something deliberate in where he looks.",
		false, 0, "", true);

	Item* galahadStatue = new Item("Galahad Statue",
		"A knight statue stands to the right, tall and noble.",
		"Sir Galahad, the pure knight. His head is turned, eyes fixed on the knight beside him.\n"
		"He does not look at the sword.",
		false, 0, "", true);

	Item* bedivereStatue = new Item("Bedivere Statue",
		"A knight statue stands to the left, his gaze locked on the stone.",
		"Sir Bedivere, the loyal. His eyes are fixed firmly on the sword in the stone,\n"
		"as if waiting to see it drawn.",
		false, 0, "", true);

	Item* runeA = new Item("Rune A",
		"A glowing rune marked with the letter A.",
		"The rune pulses faintly, etched into the base of the stone.",
		false, 0, "press the rune", true);

	Item* runeG = new Item("Rune G",
		"A glowing rune marked with the letter G.",
		"The rune pulses faintly, etched into the base of the stone.",
		false, 0, "press the rune", true);

	Item* runeB = new Item("Rune B",
		"A glowing rune marked with the letter B.",
		"The rune pulses faintly, etched into the base of the stone.",
		false, 0, "press the rune", true);

	Item* excalibur = new Item("Excalibur",
		"The legendary sword, embedded in the stone, surrounded by a shimmering forcefield.",
		"Excalibur. The blade hums with quiet power, untouchable through the shimmering barrier around the stone.",
		false, 0, "touch Excalibur", false);

	runeA->onInteract = [this](Player* p) {
		if (forcefieldDown) { std::cout << "Nothing happens.\n"; return; }
		runeSequence += "A";
		std::cout << "Rune A glows brighter.\n";
		if (runeSequence == "AGB") {
			std::cout << "The runes flare in unison. The forcefield shatters with a low hum.\n";
			forcefieldDown = true;
		}
		else if (runeSequence.length() >= 3) {
			std::cout << "The runes dim. The sequence resets.\n";
			runeSequence = "";
		}
		};

	runeG->onInteract = [this](Player* p) {
		if (forcefieldDown) { std::cout << "Nothing happens.\n"; return; }
		runeSequence += "G";
		std::cout << "Rune G glows brighter.\n";
		if (runeSequence == "AGB") {
			std::cout << "The runes flare in unison. The forcefield shatters with a low hum.\n";
			forcefieldDown = true;
		}
		else if (runeSequence.length() >= 3) {
			std::cout << "The runes dim. The sequence resets.\n";
			runeSequence = "";
		}
		};

	runeB->onInteract = [this](Player* p) {
		if (forcefieldDown) { std::cout << "Nothing happens.\n"; return; }
		runeSequence += "B";
		std::cout << "Rune B glows brighter.\n";
		if (runeSequence == "AGB") {
			std::cout << "The runes flare in unison. The forcefield shatters with a low hum.\n";
			forcefieldDown = true;
		}
		else if (runeSequence.length() >= 3) {
			std::cout << "The runes dim. The sequence resets.\n";
			runeSequence = "";
		}
		};

	excalibur->onTake = [this, rich, Dream, entrance, excalibur, mordred, ExcaliburRoom](Player* p) {
		if (!forcefieldDown) {
			p->inventory.remove(excalibur);
			ExcaliburRoom->AddEntity(excalibur);
			std::cout << "But The forcefield blocks your hand. The sword remains untouchable.\n";
			return;
		}
		std::cout << "You reach for Excalibur...\n";
		p->hasObjective = true;
		entrance->AddExit(new Exit(Direction::SOUTH, ExcaliburRoom, false));
		entrance->AddEntity(mordred);
		if (p->Worthy == 10 || p->TotalValue >= rich) {
			mordred->isHostile = true;
			mordred->dialogue = "So. You found it. Hand it over and walk free.";
		}
		else {
			mordred->isHostile = false;
			mordred->dialogue = "Pathetic. Not even worth my blade. Crawl back to whatever hole you came from.";
		}
		if (p->Worthy < 10) {
			p->inventory.remove(excalibur);
			ExcaliburRoom->AddEntity(excalibur);
		}
		if (p->Worthy == 10) {
			std::cout << "The sword feels solid. Real.\n";
			std::cout << "It is time to leave.\n";
		}
		else if (p->Worthy <= 5 && p->Worthy >= 1) {
			std::cout << "The sword burns cold. It does not move.\n";
			std::cout << "The chamber rejects you. It is time to leave.\n";
		}
		else if (p->Worthy >= 6 && p->Worthy <= 9) {
			std::cout << "The world dissolves around you...\n";
			p->currentRoom = Dream;
			PressEnter();
			HandleLook(p);
		}
		else if (p->Worthy <= -1) {
			std::cout << "Darkness floods your vision. You are pulled down into a dream.\n";
			p->currentRoom = Dream;
			PressEnter();
			HandleLook(p);
		}
		};

	arthurStatue->onExamine = [leverStatue, AltarEntrance](Player* p) {
		static bool revealed = false;
		if (!revealed) {
			AltarEntrance->AddEntity(leverStatue);
			revealed = true;
			std::cout << "\nSomething clicks in your mind. The broken statue at the entrance...\n";
		}
		};

	ExcaliburRoom->AddEntity(arthurStatue);
	ExcaliburRoom->AddEntity(galahadStatue);
	ExcaliburRoom->AddEntity(bedivereStatue);
	ExcaliburRoom->AddEntity(runeA);
	ExcaliburRoom->AddEntity(runeG);
	ExcaliburRoom->AddEntity(runeB);
	ExcaliburRoom->AddEntity(excalibur);

	world.AddEntity(arthurStatue);
	world.AddEntity(galahadStatue);
	world.AddEntity(bedivereStatue);
	world.AddEntity(runeA);
	world.AddEntity(runeG);
	world.AddEntity(runeB);
	world.AddEntity(excalibur);

	// Quarters Content
	Item* holyGrail = new Item("Holy Grail",
		"A golden cup rests on a pedestal, glowing faintly.",
		"The Holy Grail. Ancient power radiates from its surface. The world feels clearer just looking at it.",
		false, 6);

	NPC* merlin = new NPC("Merlin",
		"An old man stands before you, eyes burning with anger.",
		100, true, Quarters,
		"Thief! Return what is not yours!");
	merlin->damage = 20;
	merlin->worthCost = 4;

	world.AddEntity(merlin);
	world.AddEntity(holyGrail);
	Quarters->AddEntity(holyGrail);

	Item* crystalBall = new Item("Crystal Ball",
		"A crystal ball rests on a small table.",
		"The crystal ball sits quietly. A note on the desk beside it reads:\n\"I found a way to measure worthiness.\"",
		false, 0, "touch the ball", true);

	crystalBall->onInteract = [](Player* p) {
		std::cout << "The ball glows ";
		if (p->Worthy == 10) std::cout << "gold";
		else if (p->Worthy >= 6) std::cout << "yellow";
		else if (p->Worthy >= 0) std::cout << "grey";
		else std::cout << "red";
		std::cout << " at your touch.\n";
		};

	Item* scroll = new Item("Scroll",
		"A scroll lies on the floor near the bookshelf.",
		"The scroll is brittle. The writing trails off mid-sentence:\n\"...some deemed unworthy do not leave the cave.\nThe sword pulls them into a dream they never wake from.\nOnly the Grail can shatter the illusion...\"",
		false, 0);
	world.AddEntity(scroll);

	Item* bookshelf = new Item("Bookshelf",
		"A bookshelf crammed with old tomes and scrolls.",
		"You reach for a book but an invisible barrier stops you.",
		false, 0, "", true);

	bookshelf->onExamine = [scroll, Quarters](Player* p) {
		static bool revealed = false;
		if (!revealed) {
			std::cout << "As you examine the bookshelf, you notice a scroll on the ground beneath it.\n";
			Quarters->AddEntity(scroll);
			revealed = true;
		}
		};

	holyGrail->onTake = [merlin, Quarters, &world](Player* p) {
		p->hasGrail = true;
		merlin->dialogue = "Long have I waited for one such as you. Now we may speak in peace.";
		Quarters->AddEntity(merlin);
		std::cout << "\nAs you lift the Grail, the air shifts. An old man steps from the shadows.\n";
		std::cout << "\"Thief! Return what is not yours!\"\n";
		Combat(*p, *merlin, world);
		if (p->IsAlive())
			p->currentRoom->Describe();
		};

	merlin->onAct = [merlin, holyGrail, Quarters](Player* p, World* w) {
		Item* grail = nullptr;
		for (Item* item : p->inventory) {
			if (item->name == "Holy Grail") { grail = item; break; }
		}
		if (!grail) {
			std::cout << "You have nothing to offer him.\n";
			return;
		}
		std::cout << "Offer the Holy Grail back to Merlin? (y/n)\n> ";
		std::string confirm;
		std::getline(std::cin, confirm);
		if (confirm != "y" && confirm != "yes") {
			std::cout << "You keep the Grail clutched tight.\n";
			return;
		}
		std::cout << "You hold out the Grail.\n";
		std::cout << "Merlin's expression softens. He takes it gently from your hands.\n";
		p->inventory.remove(grail);
		p->TotalValue -= grail->Value;
		p->hasGrail = false;
		Quarters->AddEntity(grail);
		merlin->peacefulResolved = true;
		merlin->isHostile = false;
		merlin->combatEnded = true;
		PressEnter();
		};

	Quarters->AddEntity(bookshelf);
	world.AddEntity(bookshelf);

	Quarters->AddEntity(crystalBall);
	world.AddEntity(crystalBall);

	// RANDOM ROOM SELECTION
	std::vector<Room*> raPool = { PitRoom, NarrowTunnel, Waterfall };
	std::vector<Room*> rbPool = { Catacombs, RitualRoom };

	std::shuffle(raPool.begin(), raPool.end(), world.rng);
	std::shuffle(rbPool.begin(), rbPool.end(), world.rng);

	Room* RA1 = raPool[0];
	Room* RA2 = raPool[1];
	Room* RA3 = raPool[2];
	Room* RB1 = rbPool[0];
	Room* RB2 = rbPool[1];

	leverStatue->onInteract = [Quarters, RB2](Player* p) {
		static bool pulled = false;
		if (pulled) {
			std::cout << "The lever is already pulled.\n";
			return;
		}
		pulled = true;
		RB2->AddExit(new Exit(Direction::EAST, Quarters, false));
		Quarters->AddExit(new Exit(Direction::WEST, RB2, false));
		std::cout << "A deep rumble echoes through the cave. Something has opened, somewhere.\n";
		};

	Boar->patrolRooms = { RA1, RA2, RA3 };
	Boar->location = RA1;
	RA1->AddEntity(Boar);

	// Ending NPC
	NPC* ladyOfLake = new NPC("Lady of the Lake",
		"A figure stands upon the water, draped in white, watching you in silence.",
		150, false, Dream,
		"Come closer, traveler.");
	ladyOfLake->damage = 25;
	ladyOfLake->worthCost = 0;

	ladyOfLake->onTalk = [ladyOfLake, &world](Player* p, World* w) {
		int worth = p->Worthy;
		bool hasGrail = p->hasGrail;

		if (worth >= 6 && worth <= 9 && !hasGrail) {
			std::cout << "\nThe Lady smiles softly. She extends a hand, offering you a gleaming sword.\n";
			std::cout << "\"You have proven yourself, hero. Take it.\"\n";
			std::cout << "Your hand closes around the hilt. The world feels warm. Real.\n";
			std::cout << "...too real.\n";
			PressEnter();
		}
		else if (worth >= 6 && worth <= 9 && hasGrail) {
			std::cout << "\nThe Lady smiles softly. She extends a hand, offering you a gleaming sword.\n";
			std::cout << "Then the Grail in your pack pulses with light.\n";
			std::cout << "The lake ripples. The Lady falters. The dream tears like wet paper.\n";
			std::cout << "You stand alone in an empty chamber. The sword still in the stone.\n";
			PressEnter();
		}
		else if (worth <= 0 && !hasGrail) {
			std::cout << "\nThe Lady's smile turns cold. A throne rises from the water.\n";
			std::cout << "\"Yes... yes, this is what you wanted, isn't it? Sit. Rule. Forget.\"\n";
			std::cout << "You take the throne. The dream closes around you like a fist.\n";
			PressEnter();
		}
		else if (worth <= 0 && hasGrail) {
			std::cout << "\nThe Grail flares. The dream shatters.\n";
			std::cout << "The Lady's face twists. She knows why you are here.\n";
			std::cout << "\"So be it.\"\n";
			std::cout << "The Lady of the Lake becomes hostile!\n";
			std::cout << "!!!===YOU FEEL WEAKER (all stats -2)===!!!\n!!!===YOU CAN'T ESCAPE===!!!\n";
			p->ItemStat -= 2;
			p->Fight -= 2;
			p->Flee = -5;
			p->Evade -= 2;
			p->Act -= 2;
			p->sneakMode = false;
			ladyOfLake->isHostile = true;
			PressEnter();
			p->sneakMode = false;
			Combat(*p, *ladyOfLake, world);
			if (p->IsAlive()) {
				std::cout << "\nYou stand over what remains. Her power flows into you.\n";
				PressEnter();
			}
		}
		else {
			std::cout << "The Lady looks at you sadly. \"You should not be here.\"\n";
			return;
		}
		CheckEndingPathB(p);
		};

	ladyOfLake->onAct = [ladyOfLake](Player* p, World* w) {
		static int actCount = 0;
		actCount++;

		if (actCount == 1) {
			std::cout << "You raise the Holy Grail high. Its light burns across the lake.\n";
			std::cout << "The Lady recoils, her form flickering.\n";
			ladyOfLake->damage -= 6;
			std::cout << "(Her attacks weaken.)\n";
		}
		else if (actCount == 2) {
			std::cout << "You speak Arthur's name. The lake stills.\n";
			std::cout << "Her eyes well with tears. \"He was mine to mourn.\"\n";
			ladyOfLake->damage -= 6;
			std::cout << "(Her resolve falters.)\n";
		}
		else {
			std::cout << "She steels herself. No more pleas will reach her now.\n";
		}
		};

	Dream->AddEntity(ladyOfLake);
	world.AddEntity(ladyOfLake);

	// INVENTORY ITEM CALLBACKS

	// consumables
	potion->onInteract = [potion](Player* p) {
		if (RequireInInventory(p, potion)) return;
		p->hp = 150;
		p->inventory.remove(potion);
		p->TotalValue -= potion->Value;
		std::cout << "You drink the potion. Warmth floods your body. (HP set to 150)\n";
		};

	food->onInteract = [food](Player* p) {
		if (RequireInInventory(p, food)) return;
		p->hp += 25;
		if (p->hp > p->maxHp) p->hp = p->maxHp;
		p->inventory.remove(food);
		p->TotalValue -= food->Value;
		std::cout << "You eat the food. (+25 HP)\n";
		};

	bomb->onInteract = [bomb](Player* p) {
		if (RequireInInventory(p, bomb)) return;
		if (currentEnemy == nullptr || !currentEnemy->IsAlive()) {
			std::cout << "Nothing to throw it at.\n";
			return;
		}
		currentEnemy->TakeDamage(60);
		p->inventory.remove(bomb);
		p->TotalValue -= bomb->Value;
		std::cout << "You throw the bomb! It explodes against " << currentEnemy->name << "! (60 damage)\n";
		};

	boarMeat->onInteract = [boarMeat](Player* p) {
		if (RequireInInventory(p, boarMeat)) return;
		p->hp += 20;
		if (p->hp > p->maxHp) p->hp = p->maxHp;
		p->inventory.remove(boarMeat);
		std::cout << "You eat the boar meat. (+20 HP)\n";
		};

	Rope->onInteract = [PitRoom, Rope](Player* p) {
		if (RequireInInventory(p, Rope)) return;
		if (p->currentRoom != PitRoom) {
			std::cout << "You uncoil the rope. Nothing to use it on here.\n";
			return;
		}
		Exit* down = PitRoom->GetExit("down");
		if (down && down->isLocked) {
			down->isLocked = false;
			std::cout << "You secure the rope at the pit's edge. The path down is open.\n";
			p->inventory.remove(Rope);
		}
		else {
			std::cout << "The pit is already accessible.\n";
		}
		};

	medHerbs->onInteract = [medHerbs](Player* p) {
		if (RequireInInventory(p, medHerbs)) return;
		p->hp += 50;
		if (p->hp > p->maxHp) p->hp = p->maxHp;
		p->inventory.remove(medHerbs);
		std::cout << "You eat the medicinal herbs. (+50 HP)\n";
		};

	medHerbsWf->onInteract = [medHerbsWf](Player* p) {
		if (RequireInInventory(p, medHerbsWf)) return;
		p->hp += 50;
		if (p->hp > p->maxHp) p->hp = p->maxHp;
		p->inventory.remove(medHerbsWf);
		std::cout << "You eat the medicinal herbs. (+50 HP)\n";
		};

	poisonHerbs->onInteract = [poisonHerbs](Player* p) {
		if (RequireInInventory(p, poisonHerbs)) return;
		if (p->inCombat) {
			std::cout << "are you sure you want to eat the yellow herbs in the middle of combat? (y/n)\n> ";
			std::string confirm;
			std::getline(std::cin, confirm);
			if (confirm == "y" || confirm == "yes")
			{
				std::cout << "You lose your grip on reality mid battle.\nThe enemy takes advantage.\n\n\nWhy would you eat the poison?";
				p->inventory.remove(poisonHerbs);
				p->TakeDamage(999);
			}
			return;
		}
		p->inventory.remove(poisonHerbs);
		std::cout << "You eat the herbs. They taste bitter.\nYou pass out for several hours.\n";
		p->turnsPlayed += 15;
		};

	incense->onInteract = [incense](Player* p) {
		if (RequireInInventory(p, incense)) return;
		std::cout << "You burn the incense. you feel your sins lifting off your shoulders.\n";
		p->Worthy += 2;
		if (p->Worthy > 10) p->Worthy = 10;
		p->inventory.remove(incense);
		std::cout << "(+2 Worthy)\n";
		};

	// equipment

	ritualisticMask->buffAct = 1;
	ritualisticMask->buffItem = 1;
	ritualisticMask->buffFlee = 1;
	ritualisticMask->interact = "wear the mask";
	ritualisticMask->onInteract = [ritualisticMask](Player* p) {
		if (RequireInInventory(p, ritualisticMask)) return;
		EquipItem(p, ritualisticMask);
		};

	pristineDagger->onInteract = [pristineDagger](Player* p) {
		if (RequireInInventory(p, pristineDagger)) return;
		EquipItem(p, pristineDagger);
		};

	ornamentalSword->buffFight = 1;
	ornamentalSword->buffAct = 1;
	ornamentalSword->interact = "wield the sword";
	ornamentalSword->onInteract = [ornamentalSword](Player* p) {
		if (RequireInInventory(p, ornamentalSword)) return;
		EquipItem(p, ornamentalSword);
		};

	ancientCloak->buffItem = 1;
	ancientCloak->buffAct = 1;
	ancientCloak->interact = "wear the cloak";
	ancientCloak->onInteract = [ancientCloak](Player* p) {
		if (RequireInInventory(p, ancientCloak)) return;
		EquipItem(p, ancientCloak);
		};

	arthursRing->buffAct = 2;
	arthursRing->interact = "wear the ring";
	arthursRing->onInteract = [arthursRing](Player* p) {
		if (RequireInInventory(p, arthursRing)) return;
		EquipItem(p, arthursRing);
		};

	pristineGauntlet->buffItem = 2;
	pristineGauntlet->interact = "wear the gauntlet";
	pristineGauntlet->onInteract = [pristineGauntlet](Player* p) {
		if (RequireInInventory(p, pristineGauntlet)) return;
		EquipItem(p, pristineGauntlet);
		};

	rustyShield->buffEvade = 2;
	rustyShield->interact = "raise the shield";
	rustyShield->onInteract = [rustyShield](Player* p) {
		if (RequireInInventory(p, rustyShield)) return;
		EquipItem(p, rustyShield);
		};

	leatherArmor->buffHealth = 50;
	leatherArmor->interact = "wear the armor";
	leatherArmor->onInteract = [leatherArmor](Player* p) {
		if (RequireInInventory(p, leatherArmor)) return;
		static bool firstWear = true;
		EquipItem(p, leatherArmor);
		if (firstWear) {
			p->hp += 50;
			firstWear = false;
		}
		std::cout << "You wear the leather armor. (+50 Max HP)\n";
		};

	// EXITS

	// PitBottom <-> Pit Room
	PitRoom->AddExit(new Exit(Direction::DOWN, PitBottom, true, "Rope"));
	PitBottom->AddExit(new Exit(Direction::UP, PitRoom, false));

	// Entrance <-> RA1
	entrance->AddExit(new Exit(Direction::NORTH, RA1, false));
	RA1->AddExit(new Exit(Direction::SOUTH, entrance, false));

	// RA1 <-> RA2
	RA1->AddExit(new Exit(Direction::WEST, RA2, false));
	RA2->AddExit(new Exit(Direction::EAST, RA1, false));

	// RA2 <-> RA3
	RA2->AddExit(new Exit(Direction::NORTH, RA3, false));
	RA3->AddExit(new Exit(Direction::SOUTH, RA2, false));

	// RA2 <-> AltarEntrance
	RA2->AddExit(new Exit(Direction::WEST, AltarEntrance, false));
	AltarEntrance->AddExit(new Exit(Direction::EAST, RA2, false));

	// AltarEntrance <-> RB1
	AltarEntrance->AddExit(new Exit(Direction::WEST, RB1, false));
	RB1->AddExit(new Exit(Direction::EAST, AltarEntrance, false));

	// RB1 <-> RB2
	RB1->AddExit(new Exit(Direction::NORTH, RB2, false));
	RB2->AddExit(new Exit(Direction::SOUTH, RB1, false));

	// RB2 <-> ExcaliburRoom
	RB2->AddExit(new Exit(Direction::NORTH, ExcaliburRoom, false));
	ExcaliburRoom->AddExit(new Exit(Direction::SOUTH, RB2, false));

	// PLAYER
	player = new Player("Hero", entrance);
	player->turnsPlayed = 0;
	world.AddEntity(player);

	// Store boar for main loop
	boar = Boar;
}
