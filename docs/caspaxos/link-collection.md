# CASPAXOS concept link collection


* Original https://github.com/rystsov/caspaxos


  * **первая статья в блоге** http://rystsov.info/2015/09/16/how-paxos-works.html

  * pdf  


    1. https://github.com/rystsov/caspaxos/blob/master/latex/caspaxos.pdf   

    2. https://arxiv.org/pdf/1802.07000.pdf

  * Implementations. The algorithm is new so most implementations are actively being developed.

    - node JS async Promise wrapping hell impl model core https://github.com/gryadka/js

    - GO impl https://github.com/peterbourgon/caspaxos
      - **тут есть описание конфликтных ситуаций, хороший текст с разбором ситуаций** НАДО ПЕРЕВОД

    - ~~https://github.com/spacejam/sled/tree/master/crates/paxos~~

    - Elixir no complete impl https://github.com/ericentin/caspax

    - C# experiment https://github.com/ReubenBond/orleans/tree/poc-caspaxos

    - good GO impl wip https://github.com/komuw/kshaka  but use Acceptor  raft storage  :-)

  * Talks

    - [Papers We Love SF: Peter Bourgon on CASPaxos](https://www.youtube.com/watch?v=TW2OPHdIKsM)

  * Articles

    - доказательство линеаризуемости и корректности [Paxos on Steroids and a Crash Course in TLA+](https://tschottdorf.github.io/single-decree-paxos-tla-compare-and-swap) 

      - ~~НАДО ПЕРЕВОД~~  такое надо в оригинале изучать

    - [A TLA+ specification for Gryadka](https://medium.com/@grogepodge/tla-specification-for-gryadka-c80cd625944e)

    - [**A Proof of Correctness for CASPaxos**](http://justinjaffray.com/blog/posts/2018-04-10-caspaxos/)     /// [Перевод на Русский](blogs-02-consensus.md)   

    - [Ben Linsay on CASPaxos](https://medium.com/@blinsay/caspaxos-a8f6b3cf5515)

    - [~~Описание CASPaxos на русском~~](https://github.com/eshlykov/distributed-computing-course/blob/1c1a117a63c4b625e8ecf31e76c299efd5da3852/caspaxos.md) утеняно, веб архив тоже не содержит этот материал

    - [**Exploring consensus via python and CASPaxos**](https://www.komu.engineer/blogs/consensus)   /// [Перевод на Русский](blogs-02-consensus.md)

      - komuw - 

      - > спасибо тебе большое, ты классный

        - **Анализ консистентности на python модели** https://www.komu.engineer/blogs/02/consensus    ////  [Перевод на Русский](blogs-02-consensus.md)
        - **python model** https://github.com/komuw/casPaxos/blob/master/casPaxos.py

* Аудио подкаст https://devzen.ru/episode-0190/    //// [сам mp3 файл](https://devzen.ru/download/2018/devzen-0190-2018-05-05-b4ff2496e4b1a766.mp3)


  * доказательство линеаризуемости и корректности https://tschottdorf.github.io/single-decree-paxos-tla-compare-and-swap

    * ~~НАДО ПЕРЕВОД~~
  * разбор TLA описания https://medium.com/@grogepodge/tla-specification-for-gryadka-c80cd625944e
  * **хороший POW** https://justinjaffray.com/a-proof-of-correctness-for-caspaxos/      ///    [Русский частичный перевод](a-proof-of-correctness-for-caspaxos.md) 
  * преезентация на ютубе https://www.youtube.com/watch?v=TW2OPHdIKsM

​    

| # More docs: |                                                              |
| ------------ | ------------------------------------------------------------ |
|              | # 1. https://www.youtube.com/watch?v=SRsK-ZXTeZ0 # Paxos Simplified |
|              | # 2. https://www.youtube.com/watch?v=d7nAGI_NZPk # Google tech talks, The Paxos Algorithm |
|              | # 3. http://rystsov.info/2015/09/16/how-paxos-works.html     |
|              | # 4. https://github.com/peterbourgon/caspaxos                |
|              | # 5. https://twitter.com/komu_wairagu/status/971731937697071104 |
|              | # 6. https://twitter.com/komu_wairagu/status/971733628408467457 |
