#pragma once

#include "RE/B/BSTArray.h"
#include "RE/N/NiAVObject.h"
#include "RE/N/NiTransform.h"

namespace RE
{
    class NiNode : public NiAVObject
    {
    public:
        inline static constexpr auto RTTI = RTTI_NiNode;

        ~NiNode() override;
        NiNode* GetAsNiNode() override;
        void AddChild(NiAVObject* a_child) override;
        bool RemoveChild(NiAVObject* a_child) override;
        NiAVObject* GetObjectByName(const BSFixedString& a_name) override;
        NiAVObject* FindObjectRecursive(const BSFixedString& a_name) override;
        NiTransform* GetLocalTransform() override;
        void SetLocalTransform(const NiTransform& a_transform) override; // 38

        // members
        BSTArray<NiPointer<NiAVObject>> children;   // 130
        void*                           extraData;  // 138
        std::uint32_t                   childCount; // 140
        std::uint32_t                   flags;      // 144
        std::uint64_t                   pad148;     // 148

        static_assert(sizeof(BSTArray<NiPointer<NiAVObject>>) == 0x8);
        static_assert(offsetof(NiNode, children) == 0x130);
        static_assert(offsetof(NiNode, extraData) == 0x138);
        static_assert(offsetof(NiNode, childCount) == 0x140);
        static_assert(offsetof(NiNode, flags) == 0x144);
        static_assert(sizeof(NiNode) == 0x150);
    };
}
