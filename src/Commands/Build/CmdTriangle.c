#include <stdbool.h>
#include <math.h>

#include "ClassiCube/src/Vectors.h"
#include "ClassiCube/src/Chat.h"

#include "Draw.h"
#include "MarkSelection.h"
#include "VectorUtils.h"
#include "Brushes/Brush.h"
#include "Message.h"
#include "Parse.h"

typedef struct Triangle_ {
    FVec3 a;
    FVec3 b;
    FVec3 c;
} Triangle;

typedef struct Plane_ {
    FVec3 point;
    FVec3 normal;
} Plane;

static int GreatestInteger2(int a, int b) {
    if (a > b) {
        return a;
    }

    return b;
}

static int GreatestInteger3(int a, int b, int c) {
    return GreatestInteger2(a, GreatestInteger2(b, c));
}

static int SmallestInteger2(int a, int b) {
    if (a < b) {
        return a;
    }

    return b;
}

static int SmallestInteger3(int a, int b, int c) {
    return SmallestInteger2(a, SmallestInteger2(b, c));
}

static IVec3 MinimumVector3(IVec3 vector1, IVec3 vector2, IVec3 vector3) {
    IVec3 result;
    result.x = SmallestInteger3(vector1.x, vector2.x, vector3.x);
    result.y = SmallestInteger3(vector1.y, vector2.y, vector3.y);
    result.z = SmallestInteger3(vector1.z, vector2.z, vector3.z);

    return result;
}

static IVec3 MaximumVector3(IVec3 vector1, IVec3 vector2, IVec3 vector3) {
    IVec3 result;
    result.x = GreatestInteger3(vector1.x, vector2.x, vector3.x);
    result.y = GreatestInteger3(vector1.y, vector2.y, vector3.y);
    result.z = GreatestInteger3(vector1.z, vector2.z, vector3.z);

    return result;
}

static bool IsBetweenPlanes(IVec3 point, Plane plane1, Plane plane2, Plane plane3) {
    FVec3 floatPoint = VectorUtils_IVec3_ConvertFVec3(point);
    float dot1 = VectorUtils_FVec3_DotProduct(plane1.normal, VectorUtils_FVec3_Substract(floatPoint, plane1.point));
    float dot2 = VectorUtils_FVec3_DotProduct(plane2.normal, VectorUtils_FVec3_Substract(floatPoint, plane2.point));
    float dot3 = VectorUtils_FVec3_DotProduct(plane3.normal, VectorUtils_FVec3_Substract(floatPoint, plane3.point));

    return (dot1 >= -0.5 && dot2 >= -0.5 && dot3 >= -0.5) || (dot1 <= 0.5 && dot2 <= 0.5 && dot3 <= 0.5);
}

static FVec3 PlaneProjection(IVec3 vector, Plane plane) {
    FVec3 floatVector;
    floatVector.x = (float)vector.x;
    floatVector.y = (float)vector.y;
    floatVector.z = (float)vector.z;

    FVec3 planePointVector = VectorUtils_FVec3_Substract(floatVector, plane.point);
    float dotProduct = VectorUtils_FVec3_DotProduct(plane.normal, planePointVector);
    FVec3 multipliedNormal = VectorUtils_FVec3_ScalarMultiply(plane.normal, dotProduct);

    return VectorUtils_FVec3_Substract(floatVector, multipliedNormal);
}

static FVec3 CrossProduct(FVec3 u, FVec3 v) {
    FVec3 result;
    result.x = u.y * v.z - u.z * v.y;
    result.y = u.z * v.x - u.x * v.z;
    result.z = u.x * v.y - u.y * v.x;
    return result;
}

static FVec3 Normalize(FVec3 vector) {
    float norm = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);

    if (norm == 0.0f) {
        return vector;
    }

    return VectorUtils_FVec3_ScalarMultiply(vector, 1 / norm);
}

static bool IsAlmostInPlane(IVec3 vector, Plane plane) {
    #define EPSILON 0.5f
    FVec3 projection = PlaneProjection(vector, plane);
    FVec3 floatVector = VectorUtils_IVec3_ConvertFVec3(vector);
    return VectorUtils_FVec3_Distance(floatVector, projection) <= EPSILON;
}

static Plane TriangleToPlane(Triangle triangle) {
    Plane plane;
    plane.point = triangle.a;

    FVec3 ab = VectorUtils_FVec3_Substract(triangle.b, triangle.a);
    FVec3 ac = VectorUtils_FVec3_Substract(triangle.c, triangle.a);

    plane.normal = Normalize(CrossProduct(ab, ac));

    return plane;
}

static Plane PlaneContainingTwoPointsAndOneVector(FVec3 pointA, FVec3 pointB, FVec3 vector) {
    Plane result;
    result.point = pointA;
    result.normal = Normalize(CrossProduct(vector, VectorUtils_FVec3_Substract(pointB, pointA)));
    return result;
}

static void TriangleSelectionHandler(IVec3* marks, int count) {
    IVec3 min = MinimumVector3(marks[0], marks[1], marks[2]);
    IVec3 max = MaximumVector3(marks[0], marks[1], marks[2]);

    Triangle triangle;

    triangle.a.x = (float)marks[0].x;
    triangle.a.y = (float)marks[0].y;
    triangle.a.z = (float)marks[0].z;
    triangle.b.x = (float)marks[1].x;
    triangle.b.y = (float)marks[1].y;
    triangle.b.z = (float)marks[1].z;
    triangle.c.x = (float)marks[2].x;
    triangle.c.y = (float)marks[2].y;
    triangle.c.z = (float)marks[2].z;

    Plane plane = TriangleToPlane(triangle);

    if (plane.normal.x == 0 && plane.normal.y == 0 && plane.normal.z == 0) {
        Message_Player("Cannot draw a triangle from aligned points.");

        if (MarkSelection_Repeating()) {
            MarkSelection_Make(TriangleSelectionHandler, 3, "Triangle", MACRO_MARKSELECTION_DO_REPEAT);
            return;
        }

        return;
    }

    Plane planeAB = PlaneContainingTwoPointsAndOneVector(triangle.a, triangle.b, plane.normal);
    Plane planeBC = PlaneContainingTwoPointsAndOneVector(triangle.b, triangle.c, plane.normal);
    Plane planeCA = PlaneContainingTwoPointsAndOneVector(triangle.c, triangle.a, plane.normal);

    Draw_Start("Triangle");
    IVec3 current;

    for (int x = min.x; x <= max.x; x++) {
        current.x = x;

        for (int y = min.y; y <= max.y; y++) {
            current.y = y;

            for (int z = min.z; z <= max.z; z++) {
                current.z = z;

                if (!IsAlmostInPlane(current, plane)) {
                    continue;
                }

                if (IsBetweenPlanes(current, planeAB, planeBC, planeCA)) {
                    Draw_Brush(x, y, z);
                }
            }
        }
    }

    int blocksAffected = Draw_End();

    if (MarkSelection_Repeating()) {
        Message_Selection("&aPlace or break three points to determine the vertices.");
        MarkSelection_Make(TriangleSelectionHandler, 3, "Triangle", MACRO_MARKSELECTION_DO_REPEAT);
        return;
    }

    Message_BlocksAffected(blocksAffected);
}

static bool TryParseArguments(const cc_string* args, int argsCount) {
    bool hasBlockOrBrush = (argsCount >= 1);

    if (hasBlockOrBrush) {
        if (!Parse_TryParseBlockOrBrush(&args[0], argsCount - 0)) {
            return false;
        }

        return true;
    } else {
        Brush_LoadInventory();
        return true;
    }
}

static void Triangle_Command(const cc_string* args, int argsCount) {
    bool repeat = Parse_LastArgumentIsRepeat(args, &argsCount);

    if (!TryParseArguments(args, argsCount)) {
        return;
    }

    if (repeat) {
        Message_Player("Now repeating &bTriangle&f.");
    }

    Message_Selection("&aPlace or break three points to determine the vertices.");
    MarkSelection_Make(TriangleSelectionHandler, 3, "Triangle", repeat);
}

struct ChatCommand TriangleCommand = {
    "Triangle",
    Triangle_Command,
    COMMAND_FLAG_SINGLEPLAYER_ONLY,
    {
        "&b/Triangle @ +",
        "Draws a solid triangle between 3 points.",
        NULL,
        NULL,
        NULL
    },
    NULL
};
