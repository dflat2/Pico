#include "CC_API/Game.h"

#include "UndoTree.h"
#include "WorldUtils.h"

static int s_blocksAffected;

void Draw_Start(char* description) {
	s_blocksAffected = 0;

	if (UndoTree_Enabled()) {
		UndoTree_PrepareNewNode(description);
	}
}

void Draw_Block(int x, int y, int z, BlockID block) {
	if (!IsInWorldBoundaries(x, y, z)) {
		return;
	}

	BlockID current = GetBlock(x, y, z);

	if (current == block) {
		return;
	}

	s_blocksAffected++;
	Game_UpdateBlock(x, y, z, block);

	if (UndoTree_Enabled()) {
		UndoTree_AddBlockChangeEntry(x, y, z, block - current);
	}
}

int Draw_End() {
	if (UndoTree_Enabled()) {
		UndoTree_Commit();
	}

	return s_blocksAffected;
}
