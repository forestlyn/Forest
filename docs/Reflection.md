# Forest Engine 反射实现文档

本文记录当前 Engine 项目里反射系统的真实实现：它不是 C++ 编译器级反射，而是一套基于 `MetaResolver<T>` 静态注册的元数据系统，并被编辑器 Inspector、YAML 场景序列化和 C# 脚本字段系统共同使用。

整体可以分成两条线：

```text
C++ 组件/值类型
  -> MetaResolver<T> / Reflect<T>()
  -> MetaType + MetaField
  -> Inspector 自动绘制
  -> YAML 自动序列化 / 反序列化

C# public 脚本字段
  -> Mono 扫描字段
  -> ScriptField / ScriptFieldInstance
  -> Inspector 编辑与 YAML 持久化
  -> 运行时按 UUID 恢复 Entity / Component 托管引用
```

## 1. 反射核心结构

核心文件：

- `Engine/src/Engine/Reflection/MetaStruct.h`
- `Engine/src/Engine/Reflection/Reflect.h`
- `Engine/src/Engine/Reflection/Builder.h`
- `Engine/src/Engine/Reflection/ReflectMacro.h`

### 1.1 `MetaType`

`MetaType` 描述一个可反射类型：

- `name`：类型名，组件序列化时也用它作为 YAML key。
- `kind`：类型分类，决定 UI 和序列化如何分发。
- `size`：类型大小。
- `fields`：结构体或向量内部字段列表。
- `enumValues`：枚举名和值的映射表。
- `EnumToInt / EnumFromInt`：枚举与整数互转。
- `SerializeYaml / DeserializeYaml`：类型专属 YAML 钩子，目前主要用于 `ResourceRef<T>`。

当前 `MetaKind` 支持：

```cpp
Float, Float2, Float3, Float4,
Int, Int2, Int3, Int4,
Bool, String,
Enum, Struct,
UUID, EntityRef, ResourceRef
```

### 1.2 `MetaField`

`MetaField` 描述结构体中的一个字段：

- `name`：字段名。
- `type`：字段类型对应的 `MetaType`。
- `category`：语义分类，当前有 `Value`、`EntityReference`、`AssetReference`。
- `get / getConst`：从对象实例取字段地址的函数指针。
- `flags`：序列化标记。
- `ui`：Inspector 提示，包括步进、范围、tooltip、UI kind、只读/隐藏等。
- `visible`：条件显示谓词。
- `onChanged`：字段在 Inspector 中被编辑后触发的回调。

`category` 当前主要作为语义标记保存在元数据里；实际 UI 分发主要依赖 `MetaKind` 和 `MetaUIHint::uiKind`。

### 1.3 序列化标记与 UI 标记

当前代码里要区分两类标记：

- `PropertyFlags`：序列化相关，目前只有 `Property_Serializable` 和 `Property_Transient` 真正参与逻辑。
- `MetaUIHint::uiProperty`：编辑器相关，目前有 `Editable`、`ReadOnly`、`Hidden`。

`PropertyFlags` 不负责 Inspector 只读/隐藏；只读显示由 `UIUtils::DrawValueEdit` 检查 `UIProperty::ReadOnly`。例如 `TransformComponent::dirty` 被注册为只读：

```cpp
REFLECT_FIELD(dirty)
    .UI()
    .UIPROPERTY(Engine::UIProperty::ReadOnly)
    .FLAGS(PropertyFlags::Property_Transient);
```

`UIProperty::Hidden` 在 `DrawMetaType` 入口统一检查。如果字段被标记为 Hidden，会直接返回 `false`，表示没有发生编辑变化，同时父级 `Struct` 会继续绘制后续字段。

### 1.4 `onChanged` 字段回调

`MetaField::onChanged` 是当前新增的反射回调。它的目的不是序列化，而是让 Inspector 通过反射直接改字段后，宿主对象仍然有机会维护派生状态或缓存。

注册方式由 `FieldBuilder::ONCHANGED` 提供：

```cpp
template <void (Class::*Callback)()>
FieldBuilder& ONCHANGED()
{
    return OnChanged<Callback>();
}
```

回调类型是宿主类型的无参成员函数：

```cpp
void (Class::*)()
```

因此它适合做“字段值变了以后通知对象本身”的轻量动作，例如标记 dirty、重算缓存、刷新依赖数据等。

## 2. 类型注册方式

所有反射访问都从 `Reflect<T>()` 开始：

```cpp
template <typename T>
const MetaType& Reflect()
{
    return MetaResolver<T>::Get();
}
```

一个类型能被反射，前提是存在对应的 `MetaResolver<T>`。

### 2.1 基础类型特化

`Reflect.h` 已经为这些类型提供了 `MetaResolver`：

- `float`
- `int`
- `bool`
- `std::string`
- `glm::vec2 / vec3 / vec4`
- `glm::ivec2 / ivec3 / ivec4`
- `Engine::UUID`
- `Engine::EntityRef`
- `Engine::ResourceRef<T>`

向量类型不只是标记为 `Float3` / `Int2`，也会暴露内部 `x/y/z/w` 字段，方便在需要时继续递归展开。

### 2.2 组件和结构体注册

普通组件通过宏注册：

```cpp
REFLECT_TYPE_BEGIN(TransformComponent)
REFLECT_FIELD(Position);
REFLECT_FIELD(Rotation);
REFLECT_FIELD(Scale);
REFLECT_TYPE_END(TransformComponent)
```

这些宏会生成 `MetaResolver<TransformComponent>`，内部用 `BuildFields<Self>` 和 `TypeBuilder::Field<&Self::Member>()` 收集字段。

字段默认值是：

- `category = FieldCategory::Value`
- `flags = Property_Serializable`
- `ui = MetaUIHint{}`

链式配置由 `FieldBuilder` 完成，常见写法：

```cpp
REFLECT_FIELD(Color).UIKIND(UIKind::UITYPE_Color);

REFLECT_FIELD(TextureRef)
    .Category(FieldCategory::AssetReference)
    .UIKIND(UIKind::UITYPE_Texture2D);

REFLECT_FIELD(OrthographicSize)
    .VISIBLEIF<&IsOrthographicCamera>()
    .UI()
    .TOOLTIP("Orthographic camera size.");

REFLECT_FIELD(Position).ONCHANGED<&Self::MarkDirty>();
```

当前 `FieldBuilder` 支持：

- `Flags / FLAGS`
- `Category / CATEGORY`
- `UI`
- `UIMIN`
- `UIMAX`
- `UIRANGE`
- `UIKIND`
- `UIPROPERTY`
- `Tooltip / TOOLTIP`
- `VisibleIf / VISIBLEIF`
- `OnChanged / ONCHANGED`

### 2.3 枚举注册

枚举通过 `REFLECT_ENUM_*` 注册。例如 `SceneCameraProjectionType` 和 `Rigidbody2DBodyType`：

```cpp
REFLECT_ENUM_BEGIN(SceneCameraProjectionType)
REFLECT_ENUM_VALUE(Perspective)
REFLECT_ENUM_VALUE(Orthographic)
REFLECT_ENUM_END(SceneCameraProjectionType)
```

宏会生成 `MetaKind::Enum` 的 `MetaType`，并保存 `MetaEnumValue` 列表和枚举整数转换函数。UI 中用 `BeginCombo` 绘制，YAML 中优先写枚举名。

## 3. 当前已接入反射的组件

Inspector 和序列化不是自动扫描所有 ECS 组件，而是各自维护一个 `ComponentGroup` 白名单。

序列化白名单在 `Engine/src/Engine/Serialization/ComponentSerialize.h`：

```cpp
TagComponent,
TransformComponent,
CameraComponent,
SpriteComponent,
CircleComponent,
Rigidbody2DComponent,
BoxCollider2DComponent,
CircleCollider2DComponent,
ScriptComponent
```

Inspector 白名单在 `EngineEditor/src/Panels/UIUtils.h`，当前基本与序列化列表一致。

这意味着新增组件时，除了写 `REFLECT_TYPE_BEGIN/END`，还要把组件加入对应 `ComponentGroup`，否则不会自动出现在序列化或 Inspector 链路中。

## 4. Inspector 如何使用反射

主链路：

```text
SceneHierarchyPanel::DrawComponents
  -> UIUtils::DrawComponent<T>
  -> UIUtils::DrawMetaType
  -> UIUtils::DrawValueEdit / DrawEntityRefField / DrawResourceRefField
```

`DrawComponent<T>` 取出组件实例后调用：

```cpp
DrawMetaType(name, &component, Engine::Reflect<T>(), Engine::MetaUIHint{}, context);
```

`DrawMetaType` 根据 `MetaKind` 分发：

- 标量、向量、字符串、枚举、UUID、资源引用进入 `DrawValueEdit`。
- `EntityRef` 进入 `DrawEntityRefField`。
- `Struct` 遍历 `type.fields`，并递归绘制每个字段。

`Struct` 分支现在会汇总子字段是否发生变化：

```cpp
bool changed = false;
for (const auto& field : *type.fields)
{
    if (!Engine::IsFieldVisible(field, obj))
        continue;

    if (DrawMetaType(field.name, field.get(obj), *field.type, field.ui, context))
    {
        if (field.onChanged)
            field.onChanged(obj);
        changed = true;
    }
}
return changed;
```

这里的 `obj` 是当前结构体实例，所以 `field.onChanged(obj)` 调用的是字段宿主对象上的回调，而不是字段值对象上的回调。回调只在 Inspector 绘制链路中触发；YAML 反序列化直接写字段值，目前不会触发 `onChanged`。

`DrawValueEdit` 的主要映射：

- `Bool` -> `Checkbox`
- `Int` -> `DragInt`
- `Float` -> `DragFloat`
- `Float2/3/4` -> `DragFloat*` 或三轴控件
- `Float4 + UITYPE_Color` -> `ColorEdit4`
- `String` -> `InputText`
- `Enum` -> `BeginCombo`
- `ResourceRef + UITYPE_Texture2D` -> 资源拖拽按钮和纹理预览
- `UUID` -> 文本显示

条件显示通过 `MetaField::visible` 完成。例如 Camera 组件只有在 `ProjectionType` 对应时才显示正交或透视参数，只有 `FollowTarget` 为 true 时才显示 `TargetEntity`。

## 5. YAML 序列化如何使用反射

主链路：

```text
SerializeEntity
  -> SerializeComponents
  -> SerializeComponent<T>
  -> SerializeValue

DeserializeEntity
  -> DeserializeComponents
  -> DeserializeComponent<T>
  -> DeserializeValue
```

核心文件：

- `Engine/src/Engine/Serialization/EntitySerialize.cpp`
- `Engine/src/Engine/Serialization/ComponentSerialize.h`
- `Engine/src/Engine/Serialization/SerializeUtils.h`

### 5.1 写出规则

`SerializeValue` 按 `MetaKind` 分发：

- `Bool / Int / Float / String` 直接写 YAML scalar。
- `Float2/3/4` 通过 `YAML::operator<<` 写成序列。
- `Enum` 优先写枚举名，找不到名字时退化写整数。
- `Struct` 写成 map，并递归写字段。
- `UUID` 写成 UUID 值。
- `EntityRef` 写成目标实体 UUID。
- `ResourceRef` 调用 `MetaType::SerializeYaml`，当前写出 `path`。

结构体字段写出时会跳过：

- 没有 `Property_Serializable` 的字段。
- 带 `Property_Transient` 的字段。

### 5.2 读入规则

`DeserializeValue` 同样按 `MetaKind` 恢复：

- 基础值直接 `node.as<T>()`。
- `Enum` 先按名字匹配，失败后尝试按整数读入。
- `Struct` 按字段名查找子节点，缺失字段保留对象默认值。
- `UUID`、`EntityRef` 使用对应 YAML converter。
- `ResourceRef` 调用 `MetaType::DeserializeYaml`，恢复 `path`，并把 `instance` 置空。

旧场景文件缺少新增字段时不会直接失败，因为结构体反序列化会跳过不存在的 YAML key。

### 5.3 示例

`TransformComponent` 会写成：

```yaml
TransformComponent:
  Position: [0, 0, 2]
  Rotation: [0, 0, 0]
  Scale: [1, 1, 1]
```

`TransformComponent::dirty` 当前被标记为 `Property_Transient`，因此不会写入 YAML。它由 `Position / Rotation / Scale` 的 `ONCHANGED<&Self::MarkDirty>()` 在 Inspector 编辑后重新置脏。

`SpriteComponent::TextureRef` 会写成：

```yaml
SpriteComponent:
  Color: [1, 1, 1, 1]
  TilingFactor: 1
  TextureRef:
    path: Assets/Textures/Checkerboard.png
```

资源引用只保存路径，不保存资源实例；实际加载仍交给资源系统。

## 6. EntityRef 与 ResourceRef

### 6.1 `EntityRef`

`EntityRef` 是原生 C++ 反射里的实体引用：

```cpp
struct EntityRef
{
    UUID uuid{0};
};
```

它有独立的 `MetaKind::EntityRef` 和 `MetaResolver<EntityRef>`。Inspector 中由 `DrawEntityRefField` 绘制，支持从层级面板拖入实体、显示实体名、清空引用；YAML 中只保存目标实体的 UUID。

当前使用例子是 `CameraComponent::TargetEntity`：

```cpp
bool FollowTarget = false;
EntityRef TargetEntity = {};
```

### 6.2 `ResourceRef<T>`

`ResourceRef<T>` 表示资源引用：

```cpp
template <typename T>
struct ResourceRef
{
    std::string path;
    Ref<T> instance;
};
```

反射层把它标记为 `MetaKind::ResourceRef`，并暴露 `path` 字段。YAML converter 只保存 `path`，读取时不主动加载资源，而是将 `instance` 置空。

当前 Inspector 对资源引用的特殊绘制主要针对：

```cpp
ResourceRef<Renderer::Texture2D>
```

并依赖字段配置：

```cpp
.UIKIND(UIKind::UITYPE_Texture2D)
```

## 7. C# 脚本字段反射

C# 脚本字段不走 `MetaType / MetaField`，而是脚本系统自己的字段反射。

核心文件：

- `Engine/src/Engine/Scripts/ScriptEngine.h`
- `Engine/src/Engine/Scripts/ScriptEngine.cpp`
- `Engine/src/Engine/Scripts/Utils.cpp`
- `Engine/src/Engine/Serialization/EntitySerialize.cpp`
- `EngineEditor/src/Panels/UIUtils.cpp`

### 7.1 字段扫描

`ScriptEngine::LoadAssemblyClasses` 遍历 Mono 程序集里的类型，只收集 `Engine.Entity` 子类作为脚本类，并扫描其 public 字段。

字段类型通过 `MonoTypeToScriptFieldType` 映射：

- C# 基础数值、bool、char。
- `Engine.Vector2 / Vector3 / Vector4`。
- `Engine.Entity` 或其子类 -> `ScriptFieldType::Entity`。
- `Engine.Component` 子类 -> `ScriptFieldType::Component`。

字段元数据保存为 `ScriptField`：

- `Name`
- `FieldType`
- `MonoField`
- `DefaultValue`
- `TypeName`

每个实体实例化时，对应的编辑值保存为 `ScriptFieldInstance`。其内部缓冲区大小是 `MaxScriptFieldBufferSize = 16`，足够保存当前支持的值类型和 UUID。

### 7.2 编辑器显示

`UIUtils::DrawScriptField` 用于编辑器态的脚本字段缓存，`UIUtils::DrawScriptInstance` 用于运行态脚本实例字段。

当前脚本字段 Inspector 已支持：

- 基础数值、bool、向量等直接编辑。
- `Entity` 字段拖入实体，并保存 UUID。
- `Component` 字段拖入实体，并通过 `ScriptEngine::CanAssignObjectReference` 检查该实体是否拥有对应组件。

因此脚本侧 `Entity` 和 `Component` 引用在编辑器里都已经有拖拽控件。它们显示上不同：`Entity` 显示实体名，`Component` 显示 `实体名.组件类型名`。

### 7.3 脚本字段序列化

`SerializeScriptFields` 会把 `ScriptComponent` 对应实体的字段缓存写到 YAML：

```yaml
Sandbox.Player_Fields:
  m_Rigidbody:
    Name: m_Rigidbody
    Type: 17
    Value: 12292112353008337462
  playerEntity:
    Name: playerEntity
    Type: 16
    Value: 12292112353008337462
```

其中：

- `Type: 16` 是 `ScriptFieldType::Entity`。
- `Type: 17` 是 `ScriptFieldType::Component`。

`Entity` 和 `Component` 都保存 UUID。对于组件字段，这个 UUID 表示“组件所在实体”，运行时再根据字段声明的组件类型重建托管组件包装对象。

反序列化时会尝试用当前脚本类字段定义校验旧数据：

- 字段不存在则跳过。
- 字段类型变化则跳过。
- 字段缺失则使用当前脚本默认值。

### 7.4 运行时引用恢复

`Scene::OnRuntimeStart` 将脚本启动拆成三阶段：

1. 为所有带 `ScriptComponent` 的实体创建 `ScriptInstance`，并先写入非对象字段。
2. 调用 `ScriptEngine::ResolveScriptReferences` 恢复 `Entity` / `Component` 引用。
3. 调用 `OnCreate`。

这个顺序保证 A 引用 B 时，B 的脚本实例已经先创建好。

对象引用恢复用到的关键函数：

- `GetEntityIDFromEntityField`：从托管 `Entity` 对象里读取 `ID`。
- `GetEntityIDFromComponentField`：从托管 `Component.Entity` 属性中读取宿主实体 `ID`。
- `CreateManagedEntityReference`：把 UUID 恢复成 `Engine.Entity` 或脚本实体子类实例引用。
- `CreateManagedComponentObject`：把 UUID 恢复成指定 C# 组件类型的托管包装对象，并设置其 `Entity` 属性。
- `CanAssignObjectReference`：编辑器拖拽时检查实体引用或组件引用是否类型兼容。

## 8. 新增字段或组件的实际步骤

### 8.1 新增 C++ 字段

如果字段类型已经有 `MetaResolver<T>`：

1. 在组件结构体中新增字段。
2. 在对应 `REFLECT_TYPE_BEGIN/END` 中加入 `REFLECT_FIELD(...)`。
3. 需要特殊 UI 时追加 `UIKIND / UIRANGE / VISIBLEIF / UIPROPERTY` 等。
4. 如果直接编辑字段后需要刷新宿主对象状态，追加 `ONCHANGED<&Self::Callback>()`。
5. 需要避免落盘时使用 `FLAGS(Property_Transient)`，或不要注册到反射字段。

完成后，Inspector 和 YAML 会自动处理它，前提是该组件已经在对应 `ComponentGroup` 白名单中。

### 8.2 新增 C++ 组件

新增组件要同时处理三处：

1. 给组件写 `REFLECT_TYPE_BEGIN/END`。
2. 加入 `ComponentSerialize.h` 的 `REFLECT_SERIALIZE_TYPE`，否则不会保存/加载。
3. 加入 `UIUtils.h` 的 `REFLECT_UI_TYPE`，并在 `UIUtils.cpp` 增加显式模板实例化，否则不会自动绘制。

如果脚本层也要识别这个组件，还要确保它在 `AllComponents` / `ScriptGlue::RegisterComponents` 链路中能注册对应 `HasComponent` 等函数，并在 `ScriptCore` 中有对应 C# 组件类型。

### 8.3 新增基础反射类型

如果字段类型还没有 `MetaResolver<T>`，需要：

1. 在 `Reflect.h` 中增加 `MetaResolver<T>` 特化。
2. 如需 Inspector 编辑，在 `UIUtils::DrawValueEdit` 或 `DrawMetaType` 中增加 `MetaKind` 分发。
3. 如需 YAML 持久化，在 `SerializeValue / DeserializeValue` 中增加分发，或提供 `SerializeYaml / DeserializeYaml` 钩子。

## 9. 当前实现边界与注意点

- 没有全局 Type Registry。反射入口是 `Reflect<T>()`，不能按字符串动态查找任意类型。
- 组件接入依赖 `ComponentGroup` 白名单，不是 ECS registry 自动枚举。
- 目前没有方法反射，只有类型、字段和枚举值。
- `PropertyFlags` 只覆盖序列化语义；编辑器只读/隐藏走 `MetaUIHint`。
- `UIProperty::Hidden` 当前已经在 `DrawMetaType` 入口统一拦截，并返回 `false` 以避免父级 `Struct` 提前停止绘制后续字段。
- `UIKind::UITYPE_FilePath` 已定义但当前没有实际绘制分支。
- `FieldCategory` 当前主要是元数据语义标记，UI 和序列化仍主要按 `MetaKind` 分发。
- `ResourceRef<T>` 当前以路径持久化，尚未接入统一 AssetHandle / GUID。
- C++ 原生反射和 C# 脚本字段反射是两套系统：前者服务组件元数据，后者服务 Mono public 字段。

## 10. 一句话总结

Forest 当前反射系统的核心价值是：组件只要声明一次字段元数据，就能被 Inspector 和 YAML 序列化复用；脚本字段则通过 Mono 扫描和 UUID 持久化补上托管对象引用的编辑与恢复能力。两条链路目标一致，但实现路径仍然分离。
