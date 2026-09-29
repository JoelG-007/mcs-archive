object MaxValueKeys {
  def main(args: Array[String]): Unit ={
    val map = Map(
      "Red" -> 1,
      "Green" -> 4,
      "Blue" -> 3,
      "Orange" -> 4
    )

    val maxValue = map.values.max

    val result = map.filter{
      case (key, value) => value == maxValue
    }.keySet

    println("Original map: " + map)
    println("The keys with the maximum value are: " + result)
  }
}