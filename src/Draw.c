#include "ClassiCube/src/Game.h"
#include "ClassiCube/src/World.h"

#include "Brushes/Brush.h"
#include "UndoTree.h"
#include "WorldUtils.h"

static int s_blocksAffected;

void Draw_Start(char* description) {
	s_blocksAffected = 0;

	if (UndoTree_Enabled()) {
		UndoTree_TryPrepareNewNode(description);
	}
}

void Draw_Block(int x, int y, int z, BlockID block) {
	if (!World_Contains(x, y, z)) {
		return;
	}

	BlockID current = World_GetBlock(x, y, z);

	if (current == block) {
		return;
	}

	s_blocksAffected++;
	Game_UpdateBlock(x, y, z, block);

	if (UndoTree_Enabled()) {
		UndoTree_AddBlockChangeEntry(x, y, z, block - current);
	}
}

void Draw_Brush(int x, int y, int z) {
	BlockID block = Brush_Paint(x, y, z);
	Draw_Block(x, y, z, block);
}

int Draw_End() {
	if (UndoTree_Enabled()) {
		UndoTree_Commit();
	}

	return s_blocksAffected;
}
