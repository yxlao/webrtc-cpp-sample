// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_DISPLAY_ITEM_H_
#define THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_DISPLAY_ITEM_H_

#include "third_party/blink/renderer/platform/geometry/int_rect.h"
#include "third_party/blink/renderer/platform/graphics/contiguous_container.h"
#include "third_party/blink/renderer/platform/graphics/paint/display_item_client.h"
#include "third_party/blink/renderer/platform/platform_export.h"
#include "third_party/blink/renderer/platform/runtime_enabled_features.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"
#include "third_party/blink/renderer/platform/wtf/assertions.h"

#if DCHECK_IS_ON()
#include "third_party/blink/renderer/platform/json/json_values.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#endif

namespace blink {

enum class PaintPhase;

class PLATFORM_EXPORT DisplayItem {
  DISALLOW_NEW();

 public:
  enum {
    // Must be kept in sync with core/paint/PaintPhase.h.
    kPaintPhaseMax = 12,
  };

  // A display item type uniquely identifies a display item of a client.
  // Some display item types can be categorized using the following directives:
  // - In enum Type:
  //   - enum value <Category>First;
  //   - enum values of the category, first of which should equal
  //     <Category>First (for ease of maintenance, the values should be in
  //     alphabetic order);
  //   - enum value <Category>Last which should be equal to the last of the enum
  //     values of the category
  // - DEFINE_CATEGORY_METHODS(<Category>) to define is<Category>Type(Type) and
  //   is<Category>() methods.
  //
  // A category or subset of a category can contain types each of which
  // corresponds to a PaintPhase:
  // - In enum Type:
  //   - enum value <Category>[<Subset>]PaintPhaseFirst;
  //   - enum value <Category>[<Subset>]PaintPhaseLast =
  //     <Category>[<Subset>]PaintPhaseFirst + PaintPhaseMax;
  // - DEFINE_PAINT_PHASE_CONVERSION_METHOD(<Category>[<Subset>]) to define
  //   paintPhaseTo<Category>[<Subset>]Type(PaintPhase) method.
  enum Type {
    kDrawingFirst,
    kDrawingPaintPhaseFirst = kDrawingFirst,
    kDrawingPaintPhaseLast = kDrawingFirst + kPaintPhaseMax,
    kBoxDecorationBackground,
    kCapsLockIndicator,
    kCaret,
    kClippingMask,
    kColumnRules,
    kDebugDrawing,
    kDocumentRootBackdrop,
    kDocumentBackground,
    kDragImage,
    kDragCaret,
    kForcedColorsModeBackplate,
    kSVGImage,
    kLinkHighlight,
    kImageAreaFocusRing,
    kOverflowControls,
    kFrameOverlay,
    kPopupContainerBorder,
    kPopupListBoxBackground,
    kPopupListBoxRow,
    kPrintedContentDestinationLocations,
    kPrintedContentPDFURLRect,
    kReflectionMask,
    kResizer,
    kSVGClip,
    kSVGMask,
    kScrollCorner,
    // The following 3 types are used during cc::Scrollbar::PaintPart() only.
    // During Paint stage of document lifecycle update, we record
    // ScrollbarDisplayItem instead of DrawingItems of these types.
    kScrollbarTrackAndButtons,
    kScrollbarThumb,
    kScrollbarTickmarks,
    kSelectionTint,
    kTableCollapsedBorders,
    kVideoBitmap,
    kWebFont,
    kWebPlugin,
    kDrawingLast = kWebPlugin,

    kForeignLayerFirst,
    kForeignLayerCanvas = kForeignLayerFirst,
    kForeignLayerDevToolsOverlay,
    kForeignLayerPlugin,
    kForeignLayerVideo,
    kForeignLayerRemoteFrame,
    kForeignLayerContentsWrapper,
    kForeignLayerLinkHighlight,
    kForeignLayerViewportScroll,
    kForeignLayerViewportScrollbar,
    kForeignLayerLast = kForeignLayerViewportScrollbar,

    kClipPaintPhaseFirst,
    kClipPaintPhaseLast = kClipPaintPhaseFirst + kPaintPhaseMax,

    kScrollPaintPhaseFirst,
    kScrollPaintPhaseLast = kScrollPaintPhaseFirst + kPaintPhaseMax,

    kSVGTransformPaintPhaseFirst,
    kSVGTransformPaintPhaseLast = kSVGTransformPaintPhaseFirst + kPaintPhaseMax,

    kSVGEffectPaintPhaseFirst,
    kSVGEffectPaintPhaseLast = kSVGEffectPaintPhaseFirst + kPaintPhaseMax,

    // The following hit test types are for paint chunks containing hit test
    // data, when we don't have an previously set explicit chunk id when
    // creating the paint chunk, or we need dedicated paint chunk for the hit
    // test data.

    // Compositor hit testing requires that layers are created and sized to
    // include content that does not paint. Hit test data ensure a layer exists
    // and is sized properly even if no content would otherwise be painted.
    kHitTest,

    // Used both for specifying the paint-order scroll location, and for non-
    // composited scroll hit testing (see: hit_test_data.h).
    kScrollHitTest,
    // Used to prevent composited scrolling on the resize handle.
    kResizerScrollHitTest,
    // Used to prevent composited scrolling on plugins with wheel handlers.
    kPluginScrollHitTest,
    // Used to prevent composited scrolling on custom scrollbars.
    kCustomScrollbarHitTest,

    // These are for paint chunks that are forced for layers.
    kLayerChunk,
    // This is used if a layer has any negative-z-index children. Otherwise the
    // foreground is in the kLayerChunk chunk.
    kLayerChunkForeground,

    // The following 2 types are For ScrollbarDisplayItem.
    kScrollbarHorizontal,
    kScrollbarVertical,

    kUninitializedType,
    kTypeLast = kUninitializedType
  };

  // Some fields are copied from |client|, because we need to access them in
  // later paint cycles when |client| may have been destroyed.
  DisplayItem(const DisplayItemClient& client,
              Type type,
              wtf_size_t derived_size,
              const IntRect& visual_rect,
              bool draws_content = false)
      : client_(&client),
        visual_rect_(visual_rect),
        fragment_(0),
        type_(type),
        derived_size_(derived_size),
        raster_effect_outset_(
            static_cast<unsigned>(client.VisualRectOutsetForRasterEffects())),
        draws_content_(draws_content),
        is_cacheable_(client.IsCacheable()),
        is_tombstone_(false) {
    // |derived_size| must fit in |derived_size_|.
    // If it doesn't, enlarge |derived_size_| and fix this assert.
    SECURITY_DCHECK(derived_size == derived_size_);
    SECURITY_DCHECK(derived_size >= sizeof(*this));
    DCHECK_EQ(client.VisualRectOutsetForRasterEffects(),
              GetRasterEffectOutset());
  }

  virtual ~DisplayItem() = default;

  // Ids are for matching new DisplayItems with existing DisplayItems.
  struct Id {
    DISALLOW_NEW();
    Id(const DisplayItemClient& client, Type type, wtf_size_t fragment = 0)
        : client(client), type(type), fragment(fragment) {}
    Id(const Id& id, wtf_size_t fragment)
        : client(id.client), type(id.type), fragment(fragment) {}

    String ToString() const;

    const DisplayItemClient& client;
    const Type type;
    const wtf_size_t fragment;
  };

  Id GetId() const { return Id(*client_, GetType(), fragment_); }

  const DisplayItemClient& Client() const {
    DCHECK(client_);
    return *client_;
  }

  // The bounding box of all pixels of this display item, in the transform space
  // of the containing paint chunk.
  const IntRect& VisualRect() const { return visual_rect_; }

  RasterEffectOutset GetRasterEffectOutset() const {
    return static_cast<RasterEffectOutset>(raster_effect_outset_);
  }

  Type GetType() const { return static_cast<Type>(type_); }

  // Size of this object in memory, used to move it with memcpy.
  // This is not sizeof(*this), because it needs to account for the size of
  // the derived class (i.e. runtime type). Derived classes are expected to
  // supply this to the DisplayItem constructor.
  wtf_size_t DerivedSize() const { return derived_size_; }

  // The fragment is part of the id, to uniquely identify display items in
  // different fragments for the same client and type.
  wtf_size_t Fragment() const { return fragment_; }
  void SetFragment(wtf_size_t fragment) { fragment_ = fragment; }

// See comments of enum Type for usage of the following macros.
#define DEFINE_CATEGORY_METHODS(Category)                           \
  static constexpr bool Is##Category##Type(Type type) {             \
    return type >= k##Category##First && type <= k##Category##Last; \
  }                                                                 \
  bool Is##Category() const { return Is##Category##Type(GetType()); }

#define DEFINE_PAINT_PHASE_CONVERSION_METHOD(Category)                         \
  static constexpr Type PaintPhaseTo##Category##Type(PaintPhase paint_phase) { \
    static_assert(                                                             \
        k##Category##PaintPhaseLast - k##Category##PaintPhaseFirst ==          \
            kPaintPhaseMax,                                                    \
        "Invalid paint-phase-based category " #Category                        \
        ". See comments of DisplayItem::Type");                                \
    return static_cast<Type>(static_cast<int>(paint_phase) +                   \
                             k##Category##PaintPhaseFirst);                    \
  }

  DEFINE_CATEGORY_METHODS(Drawing)
  DEFINE_PAINT_PHASE_CONVERSION_METHOD(Drawing)

  DEFINE_CATEGORY_METHODS(ForeignLayer)

  DEFINE_PAINT_PHASE_CONVERSION_METHOD(Clip)
  DEFINE_PAINT_PHASE_CONVERSION_METHOD(Scroll)
  DEFINE_PAINT_PHASE_CONVERSION_METHOD(SVGTransform)
  DEFINE_PAINT_PHASE_CONVERSION_METHOD(SVGEffect)

  bool IsScrollbar() const {
    return type_ == kScrollbarHorizontal || type_ == kScrollbarVertical;
  }

  bool IsCacheable() const { return is_cacheable_; }
  void SetUncacheable() { is_cacheable_ = false; }

  virtual bool Equals(const DisplayItem& other) const {
    // Failure of this DCHECK would cause bad casts in subclasses.
    SECURITY_CHECK(!is_tombstone_);
    return client_ == other.client_ && type_ == other.type_ &&
           fragment_ == other.fragment_ && derived_size_ == other.derived_size_;
  }

  // True if this DisplayItem is the tombstone/"dead display item" as part of
  // moving an item from one list to another. See the default constructor of
  // DisplayItem.
  bool IsTombstone() const { return is_tombstone_; }

  bool DrawsContent() const { return draws_content_; }

#if DCHECK_IS_ON()
  static WTF::String TypeAsDebugString(DisplayItem::Type);
  WTF::String AsDebugString() const;
  virtual void PropertiesAsJSON(JSONObject&) const;
#endif

 private:
  template <typename T, wtf_size_t alignment>
  friend class ContiguousContainer;
  friend class DisplayItemList;

  // The default DisplayItem constructor is only used by ContiguousContainer::
  // AppendByMoving() where a tombstone DisplayItem is constructed at the source
  // location. Only set draws_content_ to false and is_tombstone_ to true,
  // leaving other fields as-is so that we can get their original values.
  // |visual_rect_| and |raster_effect_outset_| are special, see
  // DisplayItemList::AppendByMoving().
  DisplayItem() : draws_content_(false), is_tombstone_(true) {}

  const DisplayItemClient* client_;
  IntRect visual_rect_;
  wtf_size_t fragment_;
  static_assert(kTypeLast < (1 << 8),
                "DisplayItem::Type should fit in uint8_t");
  unsigned type_ : 8;
  unsigned derived_size_ : 8;  // size of the actual derived class
  unsigned raster_effect_outset_ : 2;
  unsigned draws_content_ : 1;
  unsigned is_cacheable_ : 1;
  unsigned is_tombstone_ : 1;
};

inline bool operator==(const DisplayItem::Id& a, const DisplayItem::Id& b) {
  return a.client == b.client && a.type == b.type && a.fragment == b.fragment;
}

inline bool operator!=(const DisplayItem::Id& a, const DisplayItem::Id& b) {
  return !(a == b);
}

PLATFORM_EXPORT std::ostream& operator<<(std::ostream&, DisplayItem::Type);
PLATFORM_EXPORT std::ostream& operator<<(std::ostream&, const DisplayItem::Id&);
PLATFORM_EXPORT std::ostream& operator<<(std::ostream&, const DisplayItem&);

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_PLATFORM_GRAPHICS_PAINT_DISPLAY_ITEM_H_