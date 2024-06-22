# Output format
Entities available in mustache files.

## Root
| type | name |
|-|-|
| [Class\[\]](#Class) | classes |
| [Enum\[\]](#Enum) | enums |

## Enum
| type | name |
|-|-|
| [EnumValue\[\]](#EnumValue) | values |
| [Attribute\[\]](#Attribute) | attributes |
| string | enum_name |
| string | canonical_name |
| string | include |

## EnumValue
| type | name |
|-|-|
| string | name |
| long long | value |

## Class
| type | name |
|-|-|
| [BaseClass\[\]](#BaseClass) | base_classes |
| [Field\[\]](#Field) | fields |
| [Method\[\]](#Method) | methods |
| [Attribute\[\]](#Attribute) | attributes |
| string | class_name |
| string | canonical_name |
| string | include |

## BaseClass
| type | name |
|-|-|
| string | class_name |
| string | canonical_name |

## Field
| type | name |
|-|-|
| string | name |
| string | type |
| [EAccess](#EAccess) | access |

## Method
| type | name |
|-|-|
| string | name |
| string | return_type |

## Attribute
| type | name |
|-|-|
| string | value |

## EAccess
| name | value |
|------|-----|
| None | 0 |
| Readable | 1 |
| Writable | 2 |
| Mutable | ``Readable \| Writable`` |
| Const | ``Readable`` |
