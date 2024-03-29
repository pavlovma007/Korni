# Технология личного цифрового суверенитета korni3

исходники брать так 

```bash
$ git clone -b main --single-branch https://github.com/pavlovma007/Korni.git
# сборка докер образа и запуск с demo-тестом так 
$ ./quick-start.sh
```

* [Главное](docs/Главное.md)
* [Юридический взгляд на проект корни](docs/Юридический%20взгляд%20на%20проект%20корни.md)
* [korni3-FAQ](docs/korni3-FAQ.md)
* [Korni3 api](docs/Korni3.md)
* [TODO Название](docs/TODO%20Название.md)
* [Korni3 и сетевые коммуникации](docs/Korni3%20и%20сетевые%20коммуникации.md)
* [Методы синхронизации файлов](docs/Методы%20синхронизации%20файлов.md)
* [ACID, CAS , PAXOS, Согласованность](docs/paxos.md)
* Мысли по архитектре, кому вдруг интересно
  * [Korni3 - это переизобретение "ksql on Kafka" для "реальной жизни"](docs/arch%20motivations/KORNI_3%20motivation%20arch%20concept.md)
  * [Выбор внутреннего формата хранения дампов](docs/arch%20motivations/korni%20выбор%20внутреннего%20формата%20хранения%20дампов.md)
  * [Эксперименты с размером файлов](docs/arch%20motivations/Эксперименты%20с%20размером%20файлов.md)
  * [пример вывода команды repack](docs/arch%20motivations/пример%20repack.txt) - она сжимает файлы для хранения, которые уже не будут меняться. (они слабо жмуться когда просто добавляются записи)



> 1. Главная проблема , решаемая korni3 - проблема цензуры, точнее : "исключает блокировку доступа к контенту". Метод решения юридический, а не технический - все копии данных для приложений у пользователей есть локально, принадлежат пользователю, а все приложения НЕ работаю через сеть самостоятельно - поэтому их можно публично развивать, обновлять итд. С каким именно контентом они работают локально при этом не известно.  korni3 сводит коммуникацию к простому обмену файлами по сети. 
> 2. korni3 имеет просветительскую миссию. Мы бесплатно распространяем знания и курсы по сетевой безопасности, редкие книги, исторические сведения, интервью, исторические документы, резервируем доступ к сканам, полит информацию по теории власти, политическую и экономическую аналитику, компьютерные технологии, фото и видео произведений искусства, исторические подписанные и легитимные приказы, влияющие на жизнь общества итд.  Мы хотим дать людям инструмент личного цифрового суверенитета. Суверенный интернет начинается с личного суверенитета и свободы доступа к знаниям для каждого человека. 
> 3. Борьба с цензурой и модерацией на этом не заканчивается - остаётся проблема анонимности. 
>    1. korni3 предлагает не предлагает конкретный сетевой транспорт, наоборот , предлагается использовать все виды сетевого транспорта. можно использовать любые способы передачи файлов, анонимные в том числе. 
>    2. мы создадим скрипты, которые позволят использовать все возможные способы синхронизации файлов. 
>    3. полностью Анонимная передача на данный момент не в первом приоритете, но уверяю: достаточно легко реализовать анонимность например средствами вида vpn , vds servers,...  тем людям, которых это действительно волнует. Мы уважаем индивидуальную степень паранойи каждого человека. 
>    4. Важным методом борьбы с цензурой является изобретенная мной "обязывающая лицензия контента" (требуется коллективное обсуждение этой идеи и возможно, ее доработка, всё таки я не юрист по образованию и что -то мог упустить.)
> 4. Я ищу способы усилить экономику своей страны, сделать людей богаче сильнее и независимее. У нас в стране много предателей и саботажников. Я работал много лет в области науки, технологий, электроники, программ, управлял сложными проектами, создавал системы планирования, управления и управления знаниями, занимался робототехникой и др. Нам требуется запустить "настоящий народный инвестиционный процесс" для модернизации страны и себя, освободив часть связанных средств. Нужно исключить всех паразитов из производственных и торговых цепочек, убрать все комиссии, любые платежи которые мы не хотим платить итд.  Для этого надо переделать интернет торговлю, мы должны для этого даже изменить суть и название. ( и, считаю : это должно перестать быть торговлей, это может быть равнозначный обмен, в котором не будет прибыли и наценок). Но конечно люди должны решать как и чем это должно быть, поэтому в базе я просто создаю приложение для поиска поставщиков и покупателей и обеспечиваю возможности им связаться. Тут важно цензура и модерация так же не может быть осуществлена в такой системе.

