# Output format
Values available in mustache files.

## Root
| type | name |
|-|-|
| [Class\[\]](#Class) | classes |

## Class
| type | name |
|-|-|
| [Field\[\]](#Field) | fields |
| [Method\[\]](#Method) | methods |
| [Attribute\[\]](#Attribute) | attributes |
| string | class_name |
| string | canonical_name |
| string | include |

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
