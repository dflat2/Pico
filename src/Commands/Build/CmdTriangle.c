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
    result.X = SmallestInteger3(vector1.X, vector2.X, vector3.X);
    result.Y = SmallestInteger3(vector1.Y, vector2.Y, vector3.Y);
    result.Z = SmallestInteger3(vector1.Z, vector2.Z, vector3.Z);

    return result;
}

static IVec3 MaximumVector3(IVec3 vector1, IVec3 vector2, IVec3 vector3) {
    IVec3 result;
    result.X = GreatestInteger3(vector1.X, vector2.X, vector3.X);
    result.Y = GreatestInteger3(vector1.Y, vector2.Y, vector3.Y);
    result.Z = GreatestInteger3(vector1.Z, vector2.Z, vector3.Z);

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
    floatVector.X = (float)vector.X;
    floatVector.Y = (float)vector.Y;
    floatVector.Z = (float)vector.Z;

    FVec3 planePointVector = VectorUtils_FVec3_Substract(floatVector, plane.point);
    float dotProduct = VectorUtils_FVec3_DotProduct(plane.normal, planePointVector);
    FVec3 multipliedNormal = VectorUtils_FVec3_ScalarMultiply(plane.normal, dotProduct);

    return VectorUtils_FVec3_Substract(floatVector, multipliedNormal);
}

static FVec3 CrossProduct(FVec3 u, FVec3 v) {
    FVec3 result;
    result.X = u.Y * v.Z - u.Z * v.Y;
    result.Y = u.Z * v.X - u.X * v.Z;
    result.Z = u.X * v.Y - u.Y * v.X;
    return result;
}

static FVec3 Normalize(FVec3 vector) {
    float norm = sqrt(vector.X * vector.X + vector.Y * vector.Y + vector.Z * vector.Z);

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
    Draw_Start("Triangle");

    IVec3 min = MinimumVector3(marks[0], marks[1], marks[2]);
    IVec3 max = MaximumVector3(marks[0], marks[1], marks[2]);

    Triangle triangle;

    triangle.a.X = (float)marks[0].X;
    triangle.a.Y = (float)marks[0].Y;
    triangle.a.Z = (float)marks[0].Z;
    triangle.b.X = (float)marks[1].X;
    triangle.b.Y = (float)marks[1].Y;
    triangle.b.Z = (float)marks[1].Z;
    triangle.c.X = (float)marks[2].X;
    triangle.c.Y = (float)marks[2].Y;
    triangle.c.Z = (float)marks[2].Z;

    Plane plane = TriangleToPlane(triangle);

    if (plane.normal.X == 0 && plane.normal.Y == 0 && plane.normal.Z == 0) {
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

    IVec3 current;

    for (int x = min.X; x <= max.X; x++) {
        current.X = x;

        for (int y = min.Y; y <= max.Y; y++) {
            current.Y = y;

            for (int z = min.Z; z <= max.Z; z++) {
                current.Z = z;

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
