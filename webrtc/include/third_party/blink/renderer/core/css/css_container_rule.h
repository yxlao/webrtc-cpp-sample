// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_CONTAINER_RULE_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_CONTAINER_RULE_H_

#include "third_party/blink/renderer/core/css/css_condition_rule.h"
#include "third_party/blink/renderer/platform/wtf/casting.h"

namespace blink {

class StyleRuleContainer;

class CSSContainerRule final : public CSSConditionRule {
  DEFINE_WRAPPERTYPEINFO();

 public:
  CSSContainerRule(StyleRuleContainer*, CSSStyleSheet*);
  ~CSSContainerRule() override;

  String cssText() const override;

 private:
  CSSRule::Type GetType() const override { return kContainerRule; }
};

template <>
struct DowncastTraits<CSSContainerRule> {
  static bool AllowFrom(const CSSRule& rule) {
    return rule.GetType() == CSSRule::kContainerRule;
  }
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_CSS_CSS_CONTAINER_RULE_H_