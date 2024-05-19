//
// Created on 2024/5/8.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "SvgMarker.h"
#include "properties/Rect.h"
#include "properties/ViewBox.h"
#include "utils/Utils.h"
#include <string>
#include <cstdlib>
#include <cmath>
#include <vector>

namespace rnoh {
namespace svg {

    void SvgMarker::renderMarker(OH_Drawing_Canvas *canvas, const SvgMarkerPosition& position, float strokeWidth){
        LOG(INFO) << "[SvgMarker] renderMarker start";
        const auto count = OH_Drawing_CanvasGetSaveCount(canvas);
        saveAndSetupCanvas(canvas, mCTM);
    
        if(markerTransform != nullptr) {
            OH_Drawing_MatrixDestroy(markerTransform);
        }
        markerTransform = OH_Drawing_MatrixCreate();
        Point origin = {vpToPx(position.origin.x), vpToPx(position.origin.y)};//position.origin;
        OH_Drawing_MatrixTranslate(markerTransform, origin.x, origin.y);
        double markerAngle = (mOrient == "auto") ? -1 : std::atof(mOrient.c_str());
        float degrees = 180 + (markerAngle == -1 ? position.angle : static_cast<float>(markerAngle));
        //fix me? float rad = deg2rad(angle); this code only in ios
        //degrees = SvgMarkerPositionUtils::deg2rad(degrees);
        OH_Drawing_MatrixPreRotate(markerTransform, degrees, 0, 0);

        if(mMarkerUnits == "strokeWidth"){
            OH_Drawing_MatrixPreScale(markerTransform, strokeWidth / scale_, strokeWidth / scale_, 0, 0);
        }
        if(!mAlign.empty()){
            double width = vpToPx(mMarkerWidth);
            double height = vpToPx(mMarkerHeight);
            Rect eRect(0, 0, width, height);
            Rect vbRect(vpToPx(mMinX), vpToPx(mMinY), vpToPx(mMinX + mVbWidth), vpToPx(mMinY + mVbHeight));
            OH_Drawing_Matrix* viewBoxMatrix = ViewBox::getTransform(vbRect, eRect, mAlign, mMeetOrSlice);
            float sx = OH_Drawing_MatrixGetValue(viewBoxMatrix, 0);
            float sy = OH_Drawing_MatrixGetValue(viewBoxMatrix, 4);
            OH_Drawing_MatrixPreScale(markerTransform, sx, sy, 0, 0);
        }
    
        double x = vpToPx(mRefX);
        double y = vpToPx(mRefY);
        OH_Drawing_MatrixPreTranslate(markerTransform, -x, -y);
        OH_Drawing_CanvasConcatMatrix(canvas, markerTransform);
        
        OnDrawTraversed(canvas);
    
        OH_Drawing_CanvasRestoreToCount(canvas, count);
        LOG(INFO) << "[SvgMarker] renderMarker done.";
    }

} // namespace svg
} // namespace rnoh