#include "Endings.h"
#include "Player.h"
#include "Utils.h"
#include <iostream>

void CheckEndingPathA(Player* player) {
	bool isRich = player->TotalValue >= 15;
	int w = player->Worthy;

	if (w == 10 && isRich) {
		std::cout << "\n=== Ending 3/7 - Rich King ===\n\n";
		std::cout << "  You emerge from the cave, Excalibur gleaming at your side and your pack heavy with treasure.\n";
		std::cout << "  Word spreads faster than you walk. By the time you reach the nearest village, they already know your name.\n";
		std::cout << "  Not long after, your influence grows. Lords bend the knee. People look to you for protection.\n";
		std::cout << "  You take your rightful place as king. An era of peace is sure to come.\n";
	}
	else if (w == 10 && !isRich) {
		std::cout << "\n=== Ending 4/7 - Worthy! ===\n\n";
		std::cout << "  The sword felt weightless the moment you touched it. As if it had been waiting.\n";
		std::cout << "  You walk out of the cave into the light, Excalibur in hand, and for the first time in a long while, you feel certain.\n";
		std::cout << "  Through many trials, you proved yourself worthy. Once you return, you will be hailed as a hero.\n";
		std::cout << "  Many people will soon look to you for protection. You are ready.\n";
	}
	else if (w <= 5 && isRich) {
		std::cout << "\n=== Ending 6/7 - Rich ===\n\n";
		std::cout << "  The sword rejected you. But your pack is heavy.\n";
		std::cout << "  The cave gave you other things. Good things. Things you can sell, spend, and live on.\n";
		std::cout << "  You walk out into the daylight and count your earnings.\n";
		std::cout << "  Not the legend you came for. But enough to build something real.\n";
	}
	else if (w <= 5 && !isRich) {
		std::cout << "\n=== Ending 7/7 - Unworthy ===\n\n";
		std::cout << "  You stood before the stone. The runes lit. The forcefield dropped.\n";
		std::cout << "  Your hand closed around the hilt and the sword burned cold. It didn't move.\n";
		std::cout << "  The chamber was silent. The statues watched.\n";
		std::cout << "  After all you have done, you return empty handed. The sword has judged you. You failed.\n";
	}
	else {
		std::cout << "\n=== Ending ?/7 - Coward's Way Out ===\n\n";
		std::cout << "  You turn back at the entrance. The cave yawns behind you, indifferent.\n";
		std::cout << "  No shame in it. The sword was never going anywhere.\n";
		std::cout << "  At least you got out with your life.\n";
		std::cout << "  Plenty of people who went in didn't.\n";
	}
	PressEnter();
	std::cout << "\nTurns survived: " << player->turnsPlayed << "\n";
	std::cout << "Worthiness: " << player->Worthy << "\n";
	std::cout << "Treasure collected: " << player->TotalValue << "\n";
}

void CheckEndingPathB(Player* player) {
	bool isRich = player->TotalValue >= 15;
	bool hasGrail = player->hasGrail;
	int w = player->Worthy;

	if (w >= 6 && w <= 9 && !hasGrail) {
		std::cout << "\n=== Ending 5/7 - Worthy? ===\n\n";
		std::cout << "  You hold the sword. It felt real. The lady smiled and placed it in your hands.\n";
		std::cout << "  You returned home. People celebrated. Life went on.\n";
		std::cout << "  But ever since that cave, nothing has truly felt real.\n";
		std::cout << "  The food has no taste. The laughter sounds distant. You wonder sometimes if you ever left.\n";
	}
	else if (w >= 6 && w <= 9 && hasGrail && !isRich) {
		std::cout << "\n=== Ending 5/7-S - Dream No More ===\n\n";
		std::cout << "  The lake appeared. The lady smiled. And then the Grail burned bright and the dream fell apart at the seams.\n";
		std::cout << "  You blinked. Stone walls. Darkness. The sword still in the rock.\n";
		std::cout << "  You knew it wouldn't let you take it. But you walk out with your mind your own.\n";
		std::cout << "  That is more than most who come here can say.\n";
	}
	else if (w >= 6 && w <= 9 && hasGrail && isRich) {
		std::cout << "\n=== Ending 6/7-S - Real Wealth ===\n\n";
		std::cout << "  The lake shimmered. The figure reached out. But the Grail pulsed warm and the illusion dissolved.\n";
		std::cout << "  You stood in an empty cave. The sword remained in the stone, unmoved, unjudged.\n";
		std::cout << "  Your pack is heavy. Your mind is clear.\n";
		std::cout << "  Some people chase legends. You walk out with something better. Real wealth, and the clarity to enjoy it.\n";
	}
	else if (w <= -1 && !hasGrail) {
		std::cout << "\n=== Ending 2/7 - Corrupted ===\n\n";
		std::cout << "  The dream took you before you could resist it.\n";
		std::cout << "  A dark version of everything you wanted. A throne built on ash. A sword that drips black.\n";
		std::cout << "  You never woke up.\n";
		std::cout << "  Somewhere in the cave, your body still stands at the edge of a lake, smiling at nothing.\n";
		std::cout << "  Your infamy spreads regardless. People fear what you became.\n";
		std::cout << "  But you are not there to enjoy it.\n";
	}
	else if (w <= -1 && hasGrail) {
		std::cout << "\n=== Ending 1/7 - Devil of the Lake ===\n\n";
		std::cout << "  The Grail shattered the dream before it could take hold.\n";
		std::cout << "  The lady stood at the center of the lake, unsmiling, watching you with ancient eyes.\n";
		std::cout << "  She knew why you were there.\n";
		std::cout << "  The fight was long. She was not weak.\n";
		std::cout << "  But when it was over, you took what was hers. Her power. Her sword. Corrupted now, darkened by what you are.\n";
		std::cout << "  You walk out of the cave a legend. The wrong kind.\n";
		std::cout << "  Kingdoms will whisper your name like a curse for generations to come.\n";
	}
	PressEnter();
	std::cout << "\nTurns survived: " << player->turnsPlayed << "\n";
	std::cout << "Worthiness: " << player->Worthy << "\n";
	std::cout << "Treasure collected: " << player->TotalValue << "\n";
}

void GameOver(Player* player) {
	std::cout << "\n=== GAME OVER ===\n";
	std::cout << "The darkness takes you. Your story ends here, forgotten in the depths of the cave.\n";
	std::cout << "Perhaps in another life, you would have been worthy.\n";
	PressEnter();
	std::cout << "\nTurns survived: " << player->turnsPlayed << "\n";
	std::cout << "Worthiness: " << player->Worthy << "\n";
	std::cout << "Treasure collected: " << player->TotalValue << "\n";
}
