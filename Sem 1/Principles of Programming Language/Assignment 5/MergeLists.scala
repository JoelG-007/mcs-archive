import scala.io.StdIn
object MergeLists{
  def main(args: Array[String]): Unit ={
    val list1 = List(1, 2, 3, 4)
    val list2 = List(3, 4, 5, 6)

    println("List 1: " + list1)
    println("List 2: " + list2)

    var merged = list1 ++ list2

    print("Enter a new element: ")
    val element = StdIn.readInt()

    merged = merged :+ element
    merged = merged.distinct

    println("Final List: " + merged)
  }
}