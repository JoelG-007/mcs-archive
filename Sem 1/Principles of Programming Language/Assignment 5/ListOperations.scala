object ListOperations {
  def main(args: Array[String]): Unit ={
    var list = List(1, 2, 3, 4, 5)
    println("Original List: " + list)

    list = list :+ 6
    list = list :+ 7
    list = list :+ 8

    println("After Adding Elements: " + list)

    list = list.filter(_ % 2 != 0)

    println("Final List: " + list)
  }
}