object ElementCount {
  def main(args: Array[String]): Unit ={
    val list = List(1, 2, 2, 3, 3, 3, 4, 4, 4, 4)

    val count = list.groupBy(identity).view.mapValues(_.size)

    println("Occurrences of each element:")
    println(count)
  }
}