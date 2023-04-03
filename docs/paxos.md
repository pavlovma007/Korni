## Korni3 , [CAP](https://en.wikipedia.org/wiki/CAP_theorem), [Consistency](https://en.wikipedia.org/wiki/Consistency_(database_systems)) ...

## Abstract En

* *ACM Subject Classification Information systems → Distributed storage*

* *Keywords and phrases atomic register, linearizability, paxos, consensus, wait-free*

Korni3 is a base for next level system. in this paper i discribe my implementation of 

1. wait-free, [linearizable](https://en.wikipedia.org/wiki/Linearizability), [multi-writer multi-reader](https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock) ([2nd link](https://en.wikipedia.org/wiki/Atomic_semantics))register in [unreliable](https://en.wikipedia.org/wiki/Reliability_(computer_networking))   (2nd [link](https://en.wikipedia.org/wiki/High_availability)), [asynchronous networks](https://en.wikipedia.org/wiki/Asynchronous_I/O) supporting arbitrary update operations including [compare-and-set](https://en.wikipedia.org/wiki/Compare-and-swap) (CAS)
2. interface and implementation of "[replicated state machine](https://en.wikipedia.org/wiki/State_machine_replication) (2nd [link](https://en.wikipedia.org/wiki/Replication_(computing))) (SMR) providing an interface for changing its value by applying an arbitrary user-provided function (a command). Unlike [Multi-Paxos](https://en.wikipedia.org/wiki/Paxos_(computer_science)#Multi-Paxos) and [Raft](https://en.wikipedia.org/wiki/Raft_(algorithm)) which replicate the log of commands, this impl replicates state, thus avoiding associated complexity, reducing write amplification, increasing concurrency of disk operations and hardware utilization

At this point we has only distributed key-value storage ([DHT](https://en.wikipedia.org/wiki/Distributed_hash_table)), it can change value using apply a change function, some of it logic can operate with versionized values, and with quorum (most nodes) computed value. 

Next level is a [transactions](https://en.wikipedia.org/wiki/Transaction_processing):

3. implimented proposer role extention for operate with several replicated registers
4. example: "RusLan bank" account logic with double record : one account minus AND other accaunt plus money. 

In some papers this are named a transactional manager pattern.  You can read [more about concistency here](https://en.wikipedia.org/wiki/Consensus_(computer_science)) 

"Korni", used for storage, give this garanties:

1. network and transport agnostic system. you can use any types of network communication
2. all facts and mini-documents in protocol is signed by author of korni3 account system, it can be checked while you building a network of trust

## По русски и попроще

### Гарантии

**По умолчанию** Korni3 это база в которой только добавляются новые версии записей. Каждый пользователь делает это со своей копией данных и "другие узлы" (которые не обязательны) не обязаны об этом узнавать. 

Korni3 помогает организовать синхронизацию множеств записей разных участников, как полную, так и частичную, проверяя их авторство. Это еще и ретроспекция по временным срезам.  

При этом , в зависимости от алгоритмов обработки, могут быть даны гарантий ["Консистентности в конечном счёте"](https://en.wikipedia.org/wiki/Eventual_consistency). 

Но [ACID](https://en.wikipedia.org/wiki/ACID)  гарантии тоже можно обеспечить, при желании. 

* [Atomicity](https://en.wikipedia.org/wiki/Atomicity_(database_systems)) Атомарность

  * Атомарность гарантирует, что никакая [транзакция](https://ru.wikipedia.org/wiki/Транзакция_(информатика)) не будет зафиксирована в системе частично. Будут либо выполнены все её подоперации, либо не выполнено ни одной. Поскольку на практике невозможно одновременно и атомарно выполнить всю последовательность операций внутри транзакции, вводится понятие «отката» ([rollback](https://ru.wikipedia.org/wiki/Rollback)): если транзакцию не удаётся полностью завершить, результаты всех её до сих пор произведённых действий будут отменены и система вернётся во «внешне исходное» состояние — со стороны будет казаться, что транзакции и не было (естественно, счётчики, [индексы](https://ru.wikipedia.org/wiki/Индекс_(базы_данных)) и другие внутренние структуры могут измениться, но, если СУБД запрограммирована без ошибок, это не повлияет на внешнее её поведение).

* [Consistency](https://en.wikipedia.org/wiki/Consistency_(database_systems)) Согласованность

  * Транзакция, достигающая своего нормального завершения ([англ.](https://ru.wikipedia.org/wiki/Английский_язык) *end of transaction, EOT*) и тем самым фиксирующая свои результаты, сохраняет согласованность базы данных. Другими словами, каждая успешная транзакция по определению фиксирует только допустимые результаты. Это условие является необходимым для поддержки четвёртого свойства.

    Согласованность является более широким понятием. Например, в банковской системе может существовать требование равенства суммы, списываемой с одного счёта, сумме, зачисляемой на другой. Это — бизнес-правило, и оно не может быть гарантировано только проверками целостности, его должны соблюсти программисты при написании кода транзакций. Если какая-либо транзакция произведёт списание, но не произведёт зачисления, то система останется в некорректном состоянии и свойство согласованности будет нарушено.

    Наконец, ещё одно замечание касается того, что в ходе выполнения транзакции согласованности не требуется. В нашем примере списание и зачисление будут, скорее всего, двумя разными подоперациями, и между их выполнением внутри транзакции будет видно несогласованное состояние системы. Однако не нужно забывать, что при выполнении требования изолированности (см. ниже) никаким другим транзакциям эта несогласованность не будет видна. А атомарность гарантирует, что транзакция либо будет полностью завершена, либо ни одна из операций транзакции не будет выполнена. Тем самым эта промежуточная несогласованность является скрытой.

* [Isolation](https://en.wikipedia.org/wiki/Isolation_(database_systems)) Изолированность

  * Во время выполнения транзакции параллельные транзакции не должны оказывать влияния на её результат. Изолированность — требование дорогое, поэтому в реальных базах данных существуют режимы, не полностью изолирующие транзакцию ([уровни изолированности](https://ru.wikipedia.org/wiki/Уровень_изолированности_транзакций), допускающие фантомное чтение и ниже).
  * CAS логика распределенного регистра не позволит другой транзакции работать со значением , с которым уже работает другая транзакция. 

* [Durability](https://en.wikipedia.org/wiki/Durability_(database_systems)) Устойчивость

  * Независимо от проблем на нижних уровнях (к примеру, обесточивание системы или сбои в оборудовании) изменения, сделанные успешно завершённой транзакцией, должны остаться сохранёнными после возвращения системы в работу. Другими словами, если пользователь получил подтверждение от системы, что транзакция выполнена, он может быть уверен, что сделанные им изменения не будут отменены из-за какого-либо сбоя.

### Строгая консистентность, как в банках

Для реализации строгой транзакционой ACID логики требуется, например : 

1) "протокол" (sequentional functional scheme\diagram implementation)  совместного изменения регистров с состояниями, обычно используется DRR distributed replicatable registers.   
2) [ ] ACID для нескольких ячеек сразу. WIP TODO

для реализации доступности в распределенной среде требуется : 

1. реализация распределенного регистра. Для этого сейчас я реализовал один из новых алгоритмов управления распределенными регистрами CASPaxos. 
2. [x] ACID для одной ячейки

[Сборник ссылок на теорию caspaxos, подборка здесь](caspaxos/link-collection.md)



PS 

> CASPaxos is not PAXOS. But it's  no wrong!  - me
