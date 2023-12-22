# BlueprintVariableMetadata
## Edit MetaData on Blueprint variables. Add EditCondition, DisplayAfter etc.

Tested on UE5.2

This functionality is exposed to Blueprints for cosmetic and convinience use only.

Use for metas that affect logic or blueprint compilation is discouraged and should be kept in Native classes.


### Common use cases:
- Ordering properties in child classes
- Adding edit conditions
- Using 'Categories' meta with GameplayTags
- Hiding thumbnails
- Class and Asset picker filtering
- Adding GetOptions to String selectors( Must have one return parameter named 'ReturnValue')

Adds button to Advanced category in blueprint variables. Options are filtered by Variable, Container, Blueprint types

![AddPopup](https://github.com/Bargestt/BlueprintVariableMetadata/assets/13734283/77d09288-e79b-47bb-943b-9eb01214463a)


Provides User created options and exposes many from the Engine(can be overriden by User defined option)

![Setting Screen](https://github.com/Bargestt/BlueprintVariableMetadata/assets/13734283/f017277e-cc12-4bb7-a233-1d08e077da86)


